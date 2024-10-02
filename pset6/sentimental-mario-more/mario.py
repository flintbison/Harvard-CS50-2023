# Import cs50 module to allow use of get_int.
import cs50

# Give variable "height" initial value of 0.
height = 0

# Use while loop to enforce input of value between 1 and 8 inclusive.
# If value more or less than these, then repeat prompt until accepted value breaks loop.
while height < 1 or height > 8:
    # Assign get_int value to variable "height".
    height = cs50.get_int("Provide a height for pyramid, between 1 and 8: ")

# For each row in pyramid repeat the same steps to build the row. Start range at 1 and end at height+1.
for row in range(1, height+1):

    # Iteratively add the correct amount of leading spaces before blocks for this row in pyramid.
    for leading_space in range(height - row):
        print(" ", end="")

    # Print the correct amount of blocks for the left hand pyramid for this row.
    for block in range(row):
        print("#", end="")

    # Print the two space gap between pyramids.
    print("  ", end="")

    # Print the same amount of blocks on the right side pyramid of this row.
    for block in range(row):
        print("#", end="")

    # Call print without argument to output a default value of newline. Since the print calls used in for loop exclude a newline to build row inline.
    print()

