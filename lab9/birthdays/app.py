import os

from cs50 import SQL
from flask import Flask, flash, jsonify, redirect, render_template, request, session

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///birthdays.db")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/", methods=["GET", "POST"])
def index():
    if request.method == "POST":

        # Add the user's entry into the database

        # Access the variables in the POST request sent to this route and assign them to local variables of the same name
        name = request.form.get("name")
        month = request.form.get("month")
        day = request.form.get("day")

        # Run a database execution which inserts a record which uses the name, month and day in the POST request
        db.execute("INSERT INTO birthdays (name,month,day) VALUES(?,?,?)",name,month,day)

        # Redirect user to the root route (which when visited will then trigger the GET senario below)
        return redirect("/")

    else:

        # Display the entries in the database on index.html

        # Get the all the birthdays from the birthdays database via SQL and assign what is returned to a variable called birthdays
        birthdays = db.execute("SELECT * FROM birthdays")

        # Render template index.html and pass through the birthdays variable as birthdays
        return render_template("index.html", birthdays=birthdays)


