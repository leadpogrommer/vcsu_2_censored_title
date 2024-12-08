import asyncio
import json
# from asyncio.c


import websockets
from websockets.asyncio.server import ServerConnection

import cnc
from cnc import dev_man, front_man


async def ws_handler(ws: ServerConnection):
    path = ws.request.path
    print(f'Got connection from {ws.remote_address} on path {ws.request.path}')
    if path == '/device':
        await dev_man.on_new_device(ws)
    elif path == '/front':
        await front_man.on_new_device(ws)
    else:
        print('Unknown path')


async def main():
    s = await websockets.serve(ws_handler, '0.0.0.0', 8080)
    asyncio.ensure_future(cnc.prog_man.watch_files())
    await s.serve_forever()

if __name__ == '__main__':
    asyncio.run(main())