-- Keep a log of any SQL queries you execute as you solve the mystery.

-- Obtained names of tables in database using .tables

-- Obtained schema of database using .schema

-- Saved output of .schema and .tables to a txt file for quick reference


-- Queried the exact date and street given in the specification, filtered for mention of "duck" using LIKE, which narrowed results to just the one crime report we are looking for.
SELECT *
FROM crime_scene_reports
WHERE day = 28 AND month = 7
AND year = 2021 AND
street = "Humphrey Street"
AND description LIKE "%duck%";
--  (1 report)
-- Report 295: "Theft of the CS50 duck took place at 10:15am at the Humphrey Street bakery. Interviews were conducted today with three witnesses who were present at the time – each of their interview transcripts mentions the bakery."


-- Knowing from the crime_scene_report that interviewees all mention "bakery" I queried interviews for that day and using LIKE returned all which contain "bakery".
SELECT name, transcript
FROM interviews
WHERE day = 28
AND month = 7
AND year = 2021
AND transcript LIKE "%bakery%";
-- (3 interviews)
-- Ruth: "Sometime within ten minutes of the theft, I saw the thief get into a car in the bakery parking lot and drive away. If you have security footage from the bakery parking lot, you might want to look for cars that left the parking lot in that time frame."
-- Eugene: "I don't know the thief's name, but it was someone I recognized. Earlier this morning, before I arrived at Emma's bakery, I was walking by the ATM on Leggett Street and saw the thief there withdrawing some money."
-- Raymond: "As the thief was leaving the bakery, they called someone who talked to them for less than a minute. In the call, I heard the thief say that they were planning to take the earliest flight out of Fiftyville tomorrow. The thief then asked the person on the other end of the phone to purchase the flight ticket."


-- Working on hints in first interview by Ruth:
-- "Sometime within ten minutes of the theft, I saw the thief get into a car in the bakery parking lot and drive away. If you have security footage from the bakery parking lot, you might want to look for cars that left the parking lot in that time frame."
-- Find all cars that exited the carpark within 10 min of the crime (10:15) on the date of the crime. Improved query by joining bakery_security_logs with people on the licence_plate column and only selecting relevant columns. Order by name ascending.
SELECT people.name, people.license_plate, bakery_security_logs.activity FROM people
JOIN bakery_security_logs ON bakery_security_logs.license_plate = people.license_plate
WHERE day = 28
AND month = 7
AND year = 2021
AND hour = 10
AND minute >= 15
AND minute <= 25
AND activity = "exit"
ORDER BY people.name ASC;
-- (8 suspects)
-- +---------+---------------+----------+
-- |  name   | license_plate | activity |
-- +---------+---------------+----------+
-- | Barry   | 6P58WS2       | exit     |
-- | Bruce   | 94KL13X       | exit     |
-- | Diana   | 322W7JE       | exit     |
-- | Iman    | L93JTIZ       | exit     |
-- | Kelsey  | 0NTHK55       | exit     |
-- | Luca    | 4328GD8       | exit     |
-- | Sofia   | G412CB7       | exit     |
-- | Vanessa | 5P2BI95       | exit     |
-- +---------+---------------+----------+


-- Working on hints in second interview by Eugene:
-- "I don't know the thief's name, but it was someone I recognized. Earlier this morning, before I arrived at Emma's bakery, I was walking by the ATM on Leggett Street and saw the thief there withdrawing some money."
-- ATM transactions for that day on the Leggett Street atm. Improved atm query by joining atm_transactions to bank_accounts to people - filtering by date and transaction type. Order by name ascending.
SELECT people.name, atm_transactions.transaction_type
FROM people
JOIN bank_accounts ON bank_accounts.person_id = people.id
JOIN atm_transactions ON atm_transactions.account_number = bank_accounts.account_number
WHERE day = 28
AND month = 7
AND year = 2021
AND atm_location = "Leggett Street"
AND transaction_type = "withdraw"
ORDER BY people.name ASC;
--  (8 suspects by name)
-- +---------+------------------+
-- |  name   | transaction_type |
-- +---------+------------------+
-- | Benista | withdraw         |
-- | Brooke  | withdraw         |
-- | Bruce   | withdraw         |
-- | Diana   | withdraw         |
-- | Iman    | withdraw         |
-- | Kenny   | withdraw         |
-- | Luca    | withdraw         |
-- | Taylor  | withdraw         |
-- +---------+------------------+


-- Working on hints in third interview by Raymond:
-- "As the thief was leaving the bakery, they called someone who talked to them for less than a minute. In the call, I heard the thief say that they were planning to take the earliest flight out of Fiftyville tomorrow. The thief then asked the person on the other end of the phone to purchase the flight ticket."
-- Get the caller (thief placing call) for phone calls on that day in which duration is less than a minute (< 60) (assuming duration unit is in seconds). phone_calls is joined to people table to relate names to phone numbers in call. Order by name ascending.
SELECT phone_calls.caller, people.name
FROM phone_calls
JOIN people ON people.phone_number = phone_calls.caller
WHERE day = 28
AND month = 7
AND year = 2021
AND duration < 60
ORDER BY people.name ASC;
-- (9 callers by name)
-- +----------------+---------+
-- |     caller     |  name   |
-- +----------------+---------+
-- | (338) 555-6650 | Benista |
-- | (367) 555-5533 | Bruce   |
-- | (031) 555-6622 | Carina  |
-- | (770) 555-1861 | Diana   |
-- | (499) 555-9472 | Kelsey  |
-- | (499) 555-9472 | Kelsey  |
-- | (826) 555-1652 | Kenny   |
-- | (130) 555-0289 | Sofia   |
-- | (286) 555-6063 | Taylor  |
-- +----------------+---------+


-- Get passenger list for the earliest flight out of Fiftyville on the day following the crime (29th July 2021).
-- JOIN people to passengers so we can relate people by passport number and passengers to flights by flight_id.
-- Return in the subquery only flight_id of flights leaving the airport in the city of Fiftyville, order by hour in ascending order, limit to 1 as we just want the earliest flight.
SELECT people.name
FROM people
JOIN passengers ON passengers.passport_number = people.passport_number
WHERE passengers.flight_id = (
    SELECT id
    FROM flights
    WHERE origin_airport_id = (
        SELECT id
        FROM airports
        WHERE city = "Fiftyville"
        )
    AND day = 29
    AND month = 7
    AND year = 2021
    ORDER BY hour ASC
    LIMIT 1
    )
ORDER BY people.name;
-- (8 passengers on the flight by name)
-- +--------+
-- |  name  |
-- +--------+
-- | Bruce  |
-- | Doris  |
-- | Edward |
-- | Kelsey |
-- | Kenny  |
-- | Luca   |
-- | Sofia  |
-- | Taylor |
-- +--------+


-- At this point we can identify which suspect is most likely the thief!
-- Through deduction, if we look for which name is common to all query results thus far, we can see that only Bruce appears in each set of results.
-- Therefore we can assume based on the evidence at hand, that Bruce was the thief!

-- Now we are confident the thief was Bruce we can form a query that looks for him specifically on the earliest flight for the day after the robbery.
-- From there we can return the name of the city for the destination airport.
SELECT city
FROM airports
WHERE id = (
    SELECT destination_airport_id
    FROM flights
    WHERE id = (
        SELECT flight_id
        FROM passengers
        WHERE passport_number = (
            SELECT passport_number
            FROM people
            WHERE name = "Bruce"
        )
    )
    AND day = 29
    AND month = 7
    AND year = 2021
    ORDER BY hour ASC
    LIMIT 1
);
-- (1 city)
-- +---------------+
-- |     city      |
-- +---------------+
-- | New York City |
-- +---------------+


-- Finally, all that is left to sleuth is who the accomplice was that Bruce called...
-- Using a similar query to earlier where we queried the phone_calls table, instead we are selecting and joining on the receiver column,
-- We use the same time predicates as earlier but we additionally filter by caller, specifically our prime suspect, Bruce. This narrows the resulting receivers to just one.
SELECT phone_calls.receiver, people.name
FROM phone_calls
JOIN people ON people.phone_number = phone_calls.receiver
WHERE day = 28
AND month = 7
AND year = 2021
AND duration < 60
AND phone_calls.caller = (
    SELECT caller
    FROM phone_calls
    WHERE caller = (
        SELECT phone_number
        FROM people
        WHERE name = "Bruce"
    )
)
ORDER BY people.name ASC
LIMIT 1;
-- (1 call receiver)
-- +----------------+-------+
-- |    receiver    | name  |
-- +----------------+-------+
-- | (375) 555-8161 | Robin |
-- +----------------+-------+


-- This final result using "Bruce" to filter the callers gives us just one recipient...
-- The accomplice that Bruce called after the robbery is Robin!