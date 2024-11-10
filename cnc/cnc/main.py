import asyncio

import websockets


async def ws_handler(ws: websockets.WebSocketServerProtocol):
    print(f'Got connection from {ws.remote_address} on path {ws.path}')
    for msg in ws.messages:
        print(msg)


async def main():
    s = await websockets.serve(ws_handler, '0.0.0.0', 8080)
    await s.serve_forever()

if __name__ == '__main__':
    asyncio.run(main())