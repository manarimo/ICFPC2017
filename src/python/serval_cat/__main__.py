import socket
import json
import sys
import argparse
from subprocess import Popen, PIPE

HOST = "punter.inf.ed.ac.uk"

class GameClient():
    def __init__(self, host, port, name, verbose = False):
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.client.connect((host, port))
        self.name = name
        self.verbose = verbose
        self.buffer = ""

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.client.close()

    def handshake(self):
        self.send_object({"me": self.name})
        return self.recv_object()

    def send_object(self, obj):
        self.send_message(json.dumps(obj))

    def recv_object(self):
        return json.loads(self.recv_message())

    def send_message(self, message):
        header = "{0}:".format(len(message))
        self.client.send(header.encode("utf-8"))
        self.client.send(message.encode("utf-8"))
        if self.verbose:
            print("[SENT] {0}".format(message), file = sys.stderr)
    
    def recv_message(self):
        while True:
            msg = self.client.recv(4096)
            self.buffer += msg.decode("utf-8")
            colon_index = self.buffer.find(":")
            if colon_index == -1:
                continue
            msg_length = int(self.buffer[:colon_index])
            if len(self.buffer) - colon_index - 1 >= msg_length:
                result = self.buffer[colon_index + 1:colon_index + 1 + msg_length]
                self.buffer = self.buffer[colon_index + 1 + msg_length:]
                if self.verbose:
                    print("[RCVD] {0}".format(result), file = sys.stderr)
                return result

def encode_json(obj):
    string = json.dumps(obj)
    return "{0}:{1}".format(len(string), string).encode("utf-8")
    
def decode_json(bytes):
    string = bytes.decode("utf-8")
    colon_index = string.find(":")
    return json.loads(string[colon_index + 1:])

def execute_command(command, obj):
    command = ["bin/sandstar.rb"] + command;
    process = Popen(command, stdout=PIPE, stdin=PIPE)
    return decode_json(process.communicate(input=encode_json(obj))[0])

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Talk to online server")
    parser.add_argument("port", help="Port Number", type=int)
    parser.add_argument("name", help="Name of the AI", type=str)
    parser.add_argument("command", help="Command to execute", nargs='+')
    args = parser.parse_args()
    command = args.command
    
    with GameClient(HOST, args.port, args.name, True) as client:
        # Handshake
        client.handshake()
        # Set up
        setup_input = client.recv_object()
        map = setup_input["map"]
        setup_output = execute_command(command, setup_input)
        state = setup_output["state"]
        client.send_object({"ready": setup_output["ready"]})

        # Game play
        while True:
            obj = client.recv_object()
            if "timeout" in obj:
                execute_command(command, obj)
                continue

            obj["state"] = state
            result = execute_command(command, obj)
            if "stop" in obj:
                # Clean up
                break
            state = result["state"]
            del result["state"]
            client.send_object(result)
