import json
import os

from websockets.asyncio.server import ServerConnection

async def send_command(ws: ServerConnection, command: str, data: bytes):
    if len(command) != 4:
        raise Exception(f"Invalid command: {command}")
    await ws.send(command.encode() + data, text=False)

# data ignored
async def handle_progs(ws: ServerConnection, data: bytes):
    resp = bytearray()
    for prog in os.listdir("programs"):
        resp += prog.encode() + b'\0'
    await send_command(ws, 'PRGS', resp)


# data is str
async def handle_run(ws: ServerConnection, data: bytes):
    prog_name = data.decode()
    print(f'Requested prog: {prog_name}')
    prog_data = open(f'programs/{prog_name}', 'rb').read()
    await send_command(ws, 'RUN ', prog_name.encode() + b'\0' + prog_data + b'\0')

handlers = {
    'PRGS': handle_progs,
    'RUN ': handle_run,
}