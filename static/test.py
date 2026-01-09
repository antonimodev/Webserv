import sys
import os

print("Content-Type: text/plain")
print()

print("--- PYTHON CGI INPUT TEST ---")

method = os.environ.get("REQUEST_METHOD", "UNKNOWN")
print(f"Method received: {method}")

if method == "POST":
    try:
        post_body = sys.stdin.read()
        print(f"Body received:\n'{post_body}'")
    except Exception as e:
        print(f"Error reading STDIN: {e}")
else:
    print("Send a POST request to test the input.")
