import socket
from data import random_message, get_dictionary_from_stream

def connect_to_socket():
    print("Waiting to Connect")
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('localhost', 55001))
    print("Connected Succesfully")
    s.listen(10)
    conn, addr = s.accept()
    return conn, addr

def main():
    conn, addr = connect_to_socket()
    while True:
        data = conn.recv(1024)
        if not data:
            break
        try:
            decoded_data = data.decode()
            print("Values: ", get_dictionary_from_stream(decoded_data))
            message = random_message()
            output_message = str.encode(message)
        except UnicodeDecodeError:
            print("UnicodeDecodeError - can't decode, this is the raw data", data)
            output_message = str.encode("+0+0")
        conn.sendall(output_message)

if __name__ == "__main__":
    main()
