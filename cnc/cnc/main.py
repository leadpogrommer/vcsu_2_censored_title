import asyncio
import json
# from asyncio.c


import websockets
from websockets.asyncio.server import ServerConnection

from cnc.handlers import handlers

next_device_id = 0
devices: dict[int, ServerConnection] = {}



async def device_handler(ws: ServerConnection):
    global next_device_id
    print('Starting device handler')
    device_id = next_device_id
    next_device_id += 1
    devices[device_id] = ws
    async for msg in ws:
        print(f'Received msg: {msg}')
        cmd = msg[:4].decode()
        await handlers[cmd](ws, msg[4:])


    del devices[device_id]

async def ws_handler(ws: ServerConnection):
    path = ws.request.path
    print(f'Got connection from {ws.remote_address} on path {ws.request.path}')
    if path == '/device':
        await device_handler(ws)
    else:
        print('Unknown path')





async def main():
    s = await websockets.serve(ws_handler, '0.0.0.0', 8080)
    await s.serve_forever()

if __name__ == '__main__':
    asyncio.run(main())