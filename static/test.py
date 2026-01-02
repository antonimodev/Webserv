import sys
import os

# Mandatory headers for CGI
print("Content-Type: text/plain")
print()

print("--- PYTHON CGI INPUT TEST ---")

# Check if it's a POST
method = os.environ.get("REQUEST_METHOD", "UNKNOWN")
print(f"Method received: {method}")

if method == "POST":
    # Read from Standard Input (STDIN)
    # This is where your Input Pipe "magic" works
    try:
        # Read everything coming through the pipe
        post_body = sys.stdin.read()
        print(f"Body received:\n'{post_body}'")
    except Exception as e:
        print(f"Error reading STDIN: {e}")
else:
    print("Send a POST request to test the input.")
