import socket

# Define the target host and port
host = "localhost"
port = 8080

# Create a socket object
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect to the server
client_socket.connect((host, port))

# Send data to the server
message = "hello"
client_socket.send(message.encode())

# Close the socket
client_socket.close()
