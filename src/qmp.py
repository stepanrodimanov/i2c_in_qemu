import socket
import json
import time
import sys

sock = "/tmp/qmp.sock"
dev = "/machine/peripheral/i2c"

try:
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.connect(sock)
    s.recv(4096)
    s.send(b'{"execute": "qmp_capabilities"}\n')
    s.recv(4096)


    while True:
        cmd = {"execute": "qom-get", "arguments": {"path": dev, "property": "reg"}}
        s.send(json.dumps(cmd).encode() + b"\n")
        resp = json.loads(s.recv(4096).decode())
        
        if "return" in resp:
            v = resp["return"]
            print(f"[{time.strftime('%H:%M:%S')}] reg = {v} (0x{v:02x})")
        else:
            print(f"[{time.strftime('%H:%M:%S')}] Error: {resp}")
        
        time.sleep(1)

except KeyboardInterrupt:
    print("\nStopped by user")
    sys.exit(0)
except FileNotFoundError:
    print(f"Socket {sock} not found")
    sys.exit(1)
except ConnectionRefusedError:
    print("Connection refused. Is QEMU running?")
    sys.exit(1)
except Exception as e:
    print(f"Error: {e}")
    sys.exit(1)