import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""

    # Get user_id from session and assign to variable
    user_id = int(session["user_id"])

    # Query database for user's transactions, summing the shares and grouping by symbol. Then only showing summed shares over 0 as negative, which would filter symbols for which shares are no longer held.
    user_portfolio = db.execute(
        "SELECT symbol, name, sum(shares) AS shares FROM transactions WHERE user_id = ? GROUP BY symbol HAVING sum(shares) > 0 ORDER BY shares DESC",
        user_id,
    )

    # Query database for user's cash and assign to variable
    user_cash = int(
        db.execute("SELECT cash FROM users WHERE id = ?", user_id)[0]["cash"]
    )

    # Inititalise user grand total to equal user cash
    user_total = user_cash

    # Iterate over each holding of shares and add the summed value to the user grand total
    for holding in user_portfolio:
        user_total += float(holding["shares"]) * float(
            lookup(holding["symbol"])["price"]
        )

    # Render portfolio template passing the details of each holding, the user cash, the lookup function and the usd function
    return render_template(
        "portfolio.html",
        user_portfolio=user_portfolio,
        user_cash=user_cash,
        lookup=lookup,
        user_total=user_total,
        usd=usd,
    )


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure symbol was submitted
        if not request.form.get("symbol"):
            return apology("must provide symbol")

        # Ensure shares was submitted
        elif not request.form.get("shares"):
            return apology("must provide shares")

        # Store symbol and shares as variables
        symbol = request.form.get("symbol")
        shares = request.form.get("shares")

        # Ensure shares was submitted as a number
        if not shares.isnumeric():
            return apology("only numeric values accepted for shares")

        # Ensure shares was submitted as an integer
        elif not shares.isdigit():
            return apology("shares must be a whole number")

        # Ensure shares was submitted as a positive integer which is more than 0
        elif int(shares) <= 0:
            return apology("shares must be more than 0")

        else:
            # Save symbol and shares as variables
            symbol = request.form.get("symbol")
            shares = int(request.form.get("shares"))

            # Look up current details of symbol and store dict as variable
            symbol_lookup = lookup(symbol)

            # If nothing returned from lookup assume symbol is invalid
            if not symbol_lookup:
                return apology("symbol not found")
            else:
                # Store user id from session as variable
                user_id = int(session["user_id"])

                # Get current user cash and store as variable
                user_cash = float(
                    db.execute("SELECT cash FROM users WHERE id = ?", user_id)[0][
                        "cash"
                    ]
                )

                # Multiply current symbol price by number of shares requested to buy and store transaction price as variable
                transaction_price = symbol_lookup["price"] * shares

                # Check if transaction price exceeds user's available cash for transaction
                if transaction_price > user_cash:
                    return apology("Not enough cash")

                else:
                    # Calculate new cash balance by deducting transaction price from current user cash and store as variable
                    updated_cash = user_cash - transaction_price

                    # Update user record with new cash balance
                    db.execute(
                        "UPDATE users SET cash = ? WHERE id = ?", updated_cash, user_id
                    )

                    # Insert new transaction record into transactions table
                    db.execute(
                        "INSERT INTO transactions (user_id, symbol, name, shares, price) VALUES(?,?,?,?,?)",
                        user_id,
                        symbol_lookup["symbol"],
                        symbol_lookup["name"],
                        shares,
                        symbol_lookup["price"],
                    )

                    # Flash message to user, telling them that shares have been bought
                    flash("Shares bought!")

                    # Redirect user to home page
                    return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""

    # Get user_id from session and assign to variable
    user_id = int(session["user_id"])

    # Query database for user's transactions and store dict list as user history variable
    user_history = db.execute(
        "SELECT symbol, name, shares, price, datetime FROM transactions WHERE user_id = ?",
        user_id,
    )

    # Render history template and pass user history object along with usd function
    return render_template("history.html", user_history=user_history, usd=usd)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

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


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to index
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():
    """Get stock quote."""

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure symbol was submitted
        if not request.form.get("symbol"):
            return apology("must provide symbol")

        else:
            # Store symbol as variable
            symbol = request.form.get("symbol")

            # Lookup symbol and store response as quote variable
            quote = lookup(symbol)

            # Check if quote is empty, indicating no match for symbol was found
            if not quote:
                return apology("symbol not found")

            else:
                # Render the quoted template, passing the quote dict and usd function
                return render_template("quoted.html", quote=quote, usd=usd)

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("quote.html")


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


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    # Get user_id from session and assign to variable
    user_id = int(session["user_id"])

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":
        # Ensure symbol was submitted
        if not request.form.get("symbol"):
            return apology("must provide symbol")

        # Ensure shares was submitted
        elif not request.form.get("shares"):
            return apology("must provide shares")

        # Ensure shares submitted was more than 0
        elif int(request.form.get("shares")) < 1:
            return apology("shares must be more than 0")

        else:
            # Store symbol and shares submitted as variables
            sell_symbol = request.form.get("symbol")
            sell_shares = request.form.get("shares")

            # Query database for user's transactions, store the returned list dict as variable
            user_portfolio = db.execute(
                "SELECT symbol, sum(shares) AS shares FROM transactions WHERE user_id = ? GROUP BY symbol HAVING count(shares) > 0 ORDER BY shares DESC",
                user_id,
            )

            # Initialise a variable which keeps track of wether the symbol being sold passes validations, beginning value is false until validation sets it as true
            valid_symbol = False

            # Initialise a variable which is used to track the iteration on which the symbol in the transactions list dict being requested for transacting occurs
            holding_element = 0

            # Iterate over the user portfolio to find in which dict in the list of dicts the matching symbol occurs
            for i, holding in enumerate(user_portfolio):
                # If symbol is found in this list dict element
                if ("symbol", sell_symbol) in holding.items():
                    # Set holding element to iteration on which match occured for symbol
                    holding_element = int(i)
                    # Set valid symbol variable to true to indicate a match was found in user portfolio
                    valid_symbol = True
                    # Break from the loop at this point and continue
                    break

            # After the loop above has been concluded we will know if the symbol is present in the portfolio, if it is not, display an apology
            if not valid_symbol:
                return apology("You don't own shares for that symbol", 403)

            # Check if sell shares is more than actual shares held for that symbol in the user portfolio, if they are, return an apology
            if int(sell_shares) > int(user_portfolio[holding_element]["shares"]):
                return apology("You don't own that many shares", 400)

            else:
                # Lookup symbol and store returned dict as variable
                symbol_lookup = lookup(sell_symbol)

                # Get total price of shares sold by multiplying the lookup price by number of shares transacted
                sell_total = float(symbol_lookup["price"]) * float(sell_shares)

                # Query database for user's current cash balance
                user_cash = float(
                    db.execute("SELECT cash FROM users WHERE id = ?", user_id)[0][
                        "cash"
                    ]
                )

                # Calculate updated cash balance by adding the transaction amount to the current cash balance and store as variable
                updated_cash = sell_total + user_cash

                # Update users current cash with new calculated cash balance
                db.execute(
                    "UPDATE users SET cash = ? WHERE id = ?", updated_cash, user_id
                )

                # Transform the number of shares sold to a negative integer and store in variable
                transaction_shares = -abs(int(sell_shares))

                # Insert new transaction record into database, the transaction shares will be stored as a negative integer, cancelling out any positive (buy) transactions when summed in the portfolio page
                db.execute(
                    "INSERT INTO transactions (user_id, symbol, name, shares, price) VALUES(?,?,?,?,?)",
                    user_id,
                    symbol_lookup["symbol"],
                    symbol_lookup["name"],
                    transaction_shares,
                    symbol_lookup["price"],
                )

                # Flash message to user, telling them that shares have been sold
                flash("Shares sold!")

                # Redirect user to home page
                return redirect("/")

    else:
        # Query database for user's transactions and store as dict variable
        user_portfolio = db.execute(
            "SELECT symbol, sum(shares) AS shares FROM transactions WHERE user_id = ? GROUP BY symbol HAVING sum(shares) > 0 ORDER BY shares DESC",
            user_id,
        )

        # Render the sell template and pass the user portfolio dict as a variable
        return render_template("sell.html", user_portfolio=user_portfolio)


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
