import socket
from data import State, Control

def connect_to_socket():
    print("Waiting to Connect")
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind(('localhost', 55001))
    s.listen(10)
    conn, addr = s.accept()
    print("Connected Succesfully")
    return conn, addr

def main():
    conn, addr = connect_to_socket()
    state = State()
    control1 = Control(-1, state, 75, 490)
    control2 = Control(1, state, 1425, 490)
    state.get_control_objects(control1, control2)
    while True:
        data = conn.recv(1024)
        if not data:
            break
        try:
            decoded_data = data.decode()
            state.update_state(decoded_data)
            message = state.output_message()
            output_message = str.encode(message)
        except UnicodeDecodeError:
            print("UnicodeDecodeError - can't decode, this is the raw data", data)
            output_message = str.encode("+0+0")
        conn.sendall(output_message)

if __name__ == "__main__":
    main()
