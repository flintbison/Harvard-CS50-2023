# Import libraries
import os
import requests
from datetime import datetime, timezone
import dateutil.parser as dp
from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session, url_for, jsonify
from flask_session import Session
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required

# Import google API libraries
import google.oauth2.credentials
import google_auth_oauthlib.flow
import googleapiclient.discovery

# Configure application
app = Flask(__name__)

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///raincheck.db")

# Openweather API variables
OPENWEATHER_API_FORECAST_URL = 'https://api.openweathermap.org/data/2.5/forecast?'
OPENWEATHER_API_GEOCODING_URL = 'https://api.openweathermap.org/geo/1.0/direct?'
OPENWEATHER_API_KEY = 'xxxxxxxx'

# Google API variables
# This variable specifies the name of a file that contains the OAuth 2.0
# information for this application, including its client_id and client_secret.
CLIENT_SECRETS_FILE = "client_secret.json"

# This OAuth 2.0 access scope allows for full read/write access to the
# authenticated user's account and requires requests to use an SSL connection.
SCOPES = ['https://www.googleapis.com/auth/calendar']
API_SERVICE_NAME = 'calendar'
API_VERSION = 'v3'

@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response

# Root page route - this shows the forecast (and calendar events if authorized to do so)
@app.route("/")
@login_required
def index():
    """Show forecast"""

    # Get user_id from session and assign to variable.
    user_id = int(session["user_id"])

    # Query database for user's details, store the returned list dict as variable so location details can be accessed.
    user_location = db.execute(
        "SELECT location_name, location_lat, location_lon FROM users WHERE id = ?", user_id
    )

    # Check if location columns are blank for user record, if any of them are, flash the viewer that they need to set a location and redirect to the route where they can do that
    if not user_location[0]['location_lat'] or not user_location[0]['location_lon'] or not user_location[0]['location_name']:

        # Flash message to user, telling them that they need to provide a location
        flash("To request a forecast you must first set a location")

        # Redirect user to set location
        return redirect("/location")

    # Pull together variables into a params object so it can be added onto the request.
    params = {
        'lat': user_location[0]['location_lat'],
        'lon': user_location[0]['location_lon'],
        'units':'metric',
        'appid': OPENWEATHER_API_KEY
    }

    # Make request to forecast api and assign response to a variable.
    response = requests.get(OPENWEATHER_API_FORECAST_URL, params=params)

    # Make sure that response is in a json format so it can be traversed as needed.
    weather_data = response.json()

    # Specifically target the 'list' array within the weather API response to make use of the forecasts easier.
    forecasts = weather_data['list']

    # For each forecast in list of forecasts take the epoch datetime 'dt' and convert to a weekday name and a 24hr time
    # and add those as two additional attributes in each forecast object.
    for forecast in forecasts:
        date_time = datetime.fromtimestamp(forecast['dt'])
        weekday_txt = date_time.strftime("%a %d %b")
        time_txt = date_time.strftime("%X")
        forecast['weekday_txt'] = weekday_txt
        forecast['time_txt'] = time_txt

    # Initialise variables to hold min and max datetimes for forecasts
    dt_min = 0
    dt_max = 0

    # Accessing the datetime for first and last items in forecast array
    for index, value in enumerate(forecasts):
        if index == 0:
            dt_min = convert_epoch_to_rfc3339(forecasts[index]['dt'])
    
        # Saving the last item
        dt_max = convert_epoch_to_rfc3339(forecasts[index]['dt'])

    # Print min and max after the loop
    print(f"Min: {dt_min}")
    print(f"Max: {dt_max}")
 
    # Initialize variable as False
    authorized = False

     # Initialize events dict.
    events = {}

    # Check if credentials don't exist in session.
    if 'credentials' not in session:
        # If they dont, ensure variable is set to False.
        authorized = False

    # Otherwise try requesting calendar events using the credentials.
    else:
        try:
            # Load credentials from the session.
            credentials = google.oauth2.credentials.Credentials(
                **session['credentials'])

            # Assign google API to service variable.
            service = googleapiclient.discovery.build(
                API_SERVICE_NAME, API_VERSION, credentials=credentials)

            # Call the Calendar API
            print('Getting List of events between min and max forecast')
            events_result = service.events().list(calendarId='primary',
                                                timeMin=dt_min,
                                                timeMax=dt_max,
                                                singleEvents=True,
                                                orderBy='startTime').execute()
            
            events = events_result.get('items', [])

            # Set 'authorized' to True.
            authorized = True

        except Exception as e:
            # If there's an exception, print an error message
            print(f"Error: {e}")
            print("Credentials are invalid.")

            # Set 'authorized' to False.
            authorized = False

            # Flash message to user, telling them that their password has been updated
            flash(f"Credentials are invalid.: {e}")

            # Redirect user to index
            return redirect("/")
        
    if not events:
        print('No upcoming events found.')
        # Render forecast template, pass the forecast list through and current location name.
        return render_template("forecast.html", forecasts=forecasts, location_name=user_location[0]['location_name'], authorized=authorized)
    
    # Loop through all events and calculate duration(height) and difference between start of event and start of first forecast (vertical placement)
    for event in events:
        
        # Access the start and end dateTimes and send them to a function which parses RFC3339 format into epoch timestamp, then subtract end from start to get the duration
        duration = convert_rfc3339_to_epoch(event['end']['dateTime']) - convert_rfc3339_to_epoch(event['start']['dateTime'])
        
        # Print event summary and calculated duration
        print(f"Event: {event['summary']}, Duration: {duration}")
        
        # Divide the millisecond duration of the event by 50 so it can be used as a reasonable pixel height (each forecast is already set as a height of 216 which is 10800 / 50 so it is the same scaling)
        event['height'] = (duration / 50)
        
        # Subtract the start datetime of the event from the first forecast to get the relative vertical position, dividing by 50 to have the correct scaling
        event['vertical_position'] = ((convert_rfc3339_to_epoch(event['start']['dateTime']) - convert_rfc3339_to_epoch(dt_min)) / 50)
        
        # Get the start and end datetime and convert to epoch, then assign to variables
        dt_start = datetime.fromtimestamp(convert_rfc3339_to_epoch(event['start']['dateTime']))
        dt_end = datetime.fromtimestamp(convert_rfc3339_to_epoch(event['end']['dateTime']))

        # Convert the start and end datetime to formatted strings and store as variables
        start_txt = dt_start.strftime("%a %d %b %X")
        end_txt = dt_end.strftime("%a %d %b %X")
        
        # Assign start and end datetime strings to event object
        event['start_txt'] = start_txt
        event['end_txt'] = end_txt

    # Render forecast template, pass the forecast list through and current location name, also the events array
    return render_template("forecast.html", forecasts=forecasts, location_name=user_location[0]['location_name'], authorized=authorized, events=events)

# Route for setting the location for the user, this supplements the weather API request to return forecasts for a given location
@app.route("/location", methods=["GET", "POST"])
def location():
    """Set the location for the user, which the forecast will be relative to"""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure city was submitted
        if not request.form.get("city"):
            return apology("must provide city name", 400)

        # Query API for city
        params = {
            'q': request.form.get("city"),
            'limit': 5,
            'appid': OPENWEATHER_API_KEY
        }

        # Make request to API endpoint and assign response to a variable
        response = requests.get(OPENWEATHER_API_GEOCODING_URL, params=params)

        # Parse the response as a JSON object
        geocoding_data = response.json()

        # Return the location results in a template to the user so they can choose a location or search again
        return render_template("locationresults.html", geocoding_data=geocoding_data)

    # User reached route via GET (as by clicking a link or via redirect), so render template without location results allowing them to search for location
    else:
        return render_template("location.html")

# This route is purely for catching the post requests sent via location search results so the details can be persisted in user db record
@app.route("/savelocation", methods=["POST"])
@login_required
def savelocation():
    """save the correct location name and coordinates to user profile"""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure city was submitted
        if not request.form.get("location_name") or not request.form.get("location_lat") or not request.form.get("location_lon"):
            return apology("incomplete/invalid location data submitted", 400)

        # Get user_id from session and assign to variable
        user_id = int(session["user_id"])

        # Assign request arguments to variables
        location_name = request.form.get("location_name")
        location_lat = request.form.get("location_lat")
        location_lon = request.form.get("location_lon")

        # Update user record in database with new location details
        db.execute(
            "UPDATE users SET location_name = ?, location_lat = ?, location_lon = ? WHERE id = ?", location_name, location_lat, location_lon, user_id
        )

        # Flash message to user, telling them that their password has been updated
        flash("Location updated!")

        # Redirect user to index
        return redirect("/")

    # User did not reach route via POST, return an apology telling them that method is not allowed
    else:
        return apology("method not allowed", 405)

# This route manages the login process
@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 400)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 400)

        # Query database for username
        rows = db.execute(
            "SELECT * FROM users WHERE username = ?", request.form.get("username")
        )

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(
            rows[0]["hash"], request.form.get("password")
        ):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")

# This route manages the logout process
@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to index
    return redirect("/")

# This route manages the process of registering a new account
@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username")

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password")

        # Ensure password confirmation was submitted
        elif not request.form.get("confirmation"):
            return apology("must provide password confirmation")

        # Ensure password confirmation matches password
        elif not request.form.get("confirmation") == request.form.get("password"):
            return apology("passwords do not match")

        # Assign username to variable
        username = request.form.get("username")

        # Hash the password using helper function and assign it to a variable
        hashedpassword = generate_password_hash(request.form.get("password"))

        # Return any pre-existing usernames in database of users matching the one provided
        duplicatecheck = db.execute("SELECT * FROM users WHERE username = ?", username)

        # If anything was returned by database query above, it is proof of pre-existing username
        if duplicatecheck:
            return apology("username is already taken")

        else:
            # Insert new user record into database
            db.execute(
                "INSERT INTO users (username, hash) VALUES(?,?)",
                username,
                hashedpassword,
            )

            # Redirect user to login
            return redirect("/login")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("register.html")


# This route provides the means for a user to change their account password
@app.route("/changepassword", methods=["GET", "POST"])
@login_required
def changepassword():
    """User can change account password"""

    # Get user_id from session and assign to variable
    user_id = int(session["user_id"])

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure current password was submitted
        if not request.form.get("currentpassword"):
            return apology("must provide current password")

        # Ensure new password was submitted
        elif not request.form.get("newpassword"):
            return apology("must provide new password")

        # Ensure new password confirmation was submitted
        elif not request.form.get("newconfirmation"):
            return apology("must provide new password confirmation")

        # Ensure new password and new password confirmation match
        elif not request.form.get("newconfirmation") == request.form.get("newpassword"):
            return apology("passwords do not match")

        else:
            # Assign request variables to local variables
            currentpassword = request.form.get("currentpassword")
            newpassword = request.form.get("newpassword")

            # Generate a hash of the new password and store as variable
            newpasswordhash = generate_password_hash(newpassword)

            # Query database for user record, store dict as variable
            rows = db.execute("SELECT * FROM users WHERE id = ?", user_id)

            # Ensure current password provided matches the one in database using helper fuction to check password hash
            if not check_password_hash(rows[0]["hash"], currentpassword):
                return apology(
                    "current password provided does not match current password", 403
                )

            else:
                # Update user record in database with hash of new password
                db.execute(
                    "UPDATE users SET hash = ? WHERE id = ?", newpasswordhash, user_id
                )

                # Flash message to user, telling them that their password has been updated
                flash("Password updated!")

                # Render the change password form
                return render_template("changepassword.html")

    else:
        return render_template("changepassword.html")
    
# This route displays wether the user has connected their Google calendar or not, and provides the means to connect to it
@app.route("/calendarconnection")
def calendarconnection():
    """Manage calendar connection"""

    # Initialize variable as False
    authorized = False

    # Check if credentials don't exist in session.
    if 'credentials' not in session:
        # If they dont, ensure variable is set to False.
        authorized = False

    # Otherwise check the credentials.
    else:
        try:
            # Load credentials from the session.
            credentials = google.oauth2.credentials.Credentials(
                **session['credentials'])

            # Assign google API to service variable.
            service = googleapiclient.discovery.build(
                API_SERVICE_NAME, API_VERSION, credentials=credentials)

            # Make a test request (e.g., get a list of calendars)
            calendars_result = service.calendarList().list().execute()

            # If the request was successful, print the list of calendars
            print("Calendars:")
            for calendar in calendars_result.get('items', []):
                print(f"- {calendar['summary']}")
            
            print("Credentials are valid.")

            # Set 'authorized' to True.
            authorized = True

        except Exception as e:
            # If there's an exception, print an error message
            print(f"Error: {e}")
            print("Credentials are invalid.")

            # Set 'authorized' to False.
            authorized = False

    
    # Load the page template, passing the authorized variable which will be a boolean reflecting the authoriization tests above.
    return render_template("calendarconnection.html", authorized=authorized)

# This route triggers the process of requesting authorization to connect to a Google calendar
@app.route('/authorize')
def authorize():
    # Create flow instance to manage the OAuth 2.0 Authorization Grant Flow steps.
    flow = google_auth_oauthlib.flow.Flow.from_client_secrets_file(
        CLIENT_SECRETS_FILE, scopes=SCOPES)

    # The URI created here must exactly match one of the authorized redirect URIs
    # for the OAuth 2.0 client, which you configured in the API Console. If this
    # value doesn't match an authorized URI, you will get a 'redirect_uri_mismatch'
    # error.
    flow.redirect_uri = url_for('oauth2callback', _external=True)

    authorization_url, state = flow.authorization_url(
        # Enable offline access so that you can refresh an access token without
        # re-prompting the user for permission. Recommended for web server apps.
        access_type='offline',
        # Enable incremental authorization. Recommended as a best practice.
        include_granted_scopes='true')

    # Store the state so the callback can verify the auth server response.
    session['state'] = state

    return redirect(authorization_url)

# This route specificially catches the redirect which occurs when returning from Googles authorization flow
# This means a user can remove any access to their personal calendar at any time
@app.route('/oauth2callback')
def oauth2callback():
    # Specify the state when creating the flow in the callback so that it can
    # verified in the authorization server response.
    state = session['state']

    flow = google_auth_oauthlib.flow.Flow.from_client_secrets_file(
        CLIENT_SECRETS_FILE, scopes=SCOPES, state=state)
    flow.redirect_uri = url_for('oauth2callback', _external=True)

    # Use the authorization server's response to fetch the OAuth 2.0 tokens.
    authorization_response = request.url
    flow.fetch_token(authorization_response=authorization_response)

    # Store credentials in the session.
    # ACTION ITEM: In a production app, you likely want to save these
    #              credentials in a persistent database instead.
    credentials = flow.credentials
    session['credentials'] = credentials_to_dict(credentials)

    # Flash message to user, telling them that Authorization was successful.
    flash("Authorization successful.")

    # Redirect user back to calendar connection page.
    return redirect("/calendarconnection")

# This route simultaneously deletes access tokens client-side and instructs Google to revoke the credentials on their side also
@app.route('/revoke')
def revoke():
    if 'credentials' not in session:
        return ('You need to <a href="/authorize">authorize</a> before ' +
                'testing the code to revoke credentials.')

    credentials = google.oauth2.credentials.Credentials(
        **session['credentials'])

    revoke = requests.post('https://oauth2.googleapis.com/revoke',
                           params={'token': credentials.token},
                           headers={'content-type': 'application/x-www-form-urlencoded'})

    status_code = getattr(revoke, 'status_code')
    if status_code == 200:
        # Flash message to user, telling them that revoking of credentials was successful.
        flash("Credentials successfully revoked.")

        # Clear credentials stored in session.
        del session['credentials']

        # Redirect user back to calendar connection page.
        return redirect("/calendarconnection")
    else:
        # Flash message to user, telling them that an error occurred during revoking.
        flash("An error occurred while revoking credentials.")

        # Clear credentials stored in session.
        del session['credentials']

        # Redirect user back to calendar connection page.
        return redirect("/calendarconnection")

# This function parses the credentials returned from Google oauth into a dictionary object so it can be used easily elsewhere
def credentials_to_dict(credentials):
    return {'token': credentials.token,
            'refresh_token': credentials.refresh_token,
            'token_uri': credentials.token_uri,
            'client_id': credentials.client_id,
            'client_secret': credentials.client_secret,
            'scopes': credentials.scopes}

# This function is to help with the conversion of epoch timestamps which are used in the Openweather API to RFC3339 timestamps which are used by google calendar API
def convert_epoch_to_rfc3339(epoch_timestamp):
    # Convert epoch timestamp to datetime object
    dt_object = datetime.utcfromtimestamp(epoch_timestamp)
    
    # Format datetime object as RFC3339 timestamp
    rfc3339_timestamp = dt_object.replace(tzinfo=timezone.utc).isoformat()
    
    return rfc3339_timestamp

# This function is the opposite operation of the one above, it converts RFC3339 timestamps into epoch ones (milliseconds), this is to help with conversations across weather and calendar APIs
def convert_rfc3339_to_epoch(rfc3339_timestamp):
    # Parse RFC3339 timestamp
    parsed_t = dp.parse(rfc3339_timestamp)

    # Convert datetime object to epoch timestamp (milliseconds)
    epoch_timestamp = int(parsed_t.timestamp())
    
    return epoch_timestamp

if __name__ == '__main__':
    # When running locally, disable OAuthlib's HTTPs verification.
    # ACTION ITEM for developers:
    #     When running in production *do not* leave this option enabled.
    os.environ['OAUTHLIB_INSECURE_TRANSPORT'] = '1'

    # Specify a hostname and port that are set as a valid redirect URI
    # for your API project in the Google API Console.
    app.run('localhost', 8080, debug=True, ssl_context='adhoc')


    # Run the command "flask run --cert=adhoc" in terminal to run Flask with adhoc https protocol, which is required for proper functioning of Google API callback.