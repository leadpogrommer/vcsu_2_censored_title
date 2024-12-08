import json

from websockets import ConnectionClosedError
from websockets.asyncio.server import ServerConnection

import cnc


# from cnc import dev_man


class FrontMan:
    def __init__(self):
        self.next_id = 0
        self.front_conns: dict[int, ServerConnection] = {}

    async def on_new_device(self, ws: ServerConnection):
        self.front_conns[self.next_id] = ws
        my_id = self.next_id
        self.next_id += 1

        try:
            await self.front_send_device_list(my_id, list(cnc.dev_man.device_conns.keys()))
            await self.front_send_prog_list(my_id, cnc.prog_man.get_prog_list())
            async for msg in ws:
                try:
                    await self._handle_front_message(msg, ws, my_id)
                except Exception as e:
                    print(f'ERROR: request from front failed: {e}')
        except ConnectionClosedError:
            ...
        finally:
            del self.front_conns[my_id]

    async def _handle_front_message(self, msg, ws, my_id):
        payload = json.loads(msg)
        match (payload['what']):
            case 'key':
                await cnc.dev_man.device_send_key(payload['did'], payload['data'] )
            case 'run':
                await cnc.dev_man.device_run_prog(payload['did'], payload['data'])


        print(f'Got message from front: {payload}')

    async def broadcast(self, f, *args, **kwargs):
        fids = self.front_conns.keys()
        for fid in fids:
            try:
                await f(fid, *args, **kwargs)
            except Exception as e:
                print(f'Failed to send message to front {fid}: {e}')

    async def front_send_device_list(self, fid: int, devs: list[int]):
        await self.front_conns[fid].send(json.dumps({'what': 'devices', 'data': devs}))

    async def front_send_prog_list(self, id: int, progs: list[str]):
        await self.front_conns[id].send(json.dumps({'what': 'progs', 'data': progs}))

    async def front_send_task_list(self, id, did: int, data: dict):
        await self.front_conns[id].send(json.dumps({'what': 'tasks', 'did': did, 'data': data}))





