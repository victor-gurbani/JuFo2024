#!/bin/bash

# Get the directory where the script is located
script_directory="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Initialize a variable to store the total line count
total_lines=0

# Use find to get a list of all files in the script's directory and its subdirectories
find "$script_directory" -type f | while IFS= read -r file
do
    # Count the number of lines in the file
    lines=$(wc -l < "$file")

    # Print the file name and its line count
    echo "$file: $lines lines"

    # Add the line count to the total
    ((total_lines += lines))
done

# Print the total line count for all files
echo "Total lines in all files: $total_lines"
