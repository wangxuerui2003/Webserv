import http.client

# Define the server and port
host = "localhost"
port = 8080

# Create a connection to the server
conn = http.client.HTTPConnection(host, port)

# Define the request headers
headers = {
    "Host": f"{host}:{port}",
    "Transfer-Encoding": "chunked",  # Enable chunked encoding
    "Content-Type": "text/plain",    # Specify the content type
}

# Begin the request
conn.request("POST", "/chunked_cgi.py", headers=headers)

# Define the chunks of data to send
chunks = [
    "This is the first chunk.",
    "This is the second chunk.",
    "This is the final chunk.",
]

# Send each chunk with chunk size prefix
for chunk in chunks:
    chunk_size = format(len(chunk), "x")  # Convert chunk size to hexadecimal
    chunk_data = chunk + "\r\n"           # Append CRLF
    chunk_hex = chunk_size + "\r\n" + chunk_data
    conn.send(chunk_hex.encode("utf-8"))

# Send the final empty chunk to indicate the end
conn.send("0\r\n\r\n".encode("utf-8"))

# Get the response from the server
response = conn.getresponse()

# Print the response
print("Response:")
print(response.status, response.reason)
print(response.read().decode("utf-8"))

# Close the connection
conn.close()
