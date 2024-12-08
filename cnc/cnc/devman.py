from websockets import ConnectionClosedError
from websockets.asyncio.server import ServerConnection
import cnc
# from cnc import front_man


class DevMan:
    def __init__(self):
        self.next_id = 0
        self.device_conns: dict[int, ServerConnection] = {}

    async def _notify_device_list_changed(self):
        print(f'New device list: {self.device_conns.keys()}')
        await cnc.front_man.broadcast(cnc.front_man.front_send_device_list, list(self.device_conns.keys()))

    async def on_new_device(self, ws: ServerConnection):
        self.device_conns[self.next_id] = ws
        my_id = self.next_id
        self.next_id += 1
        await self._notify_device_list_changed()

        try:
            async for msg in ws:
                await self._handle_device_message(msg, ws, my_id)
        except ConnectionClosedError:
            ...
        finally:
            del self.device_conns[my_id]
            print(f'Device {my_id} disconnected')
            await self._notify_device_list_changed()

    async def send_cmd(self, did: int, c: str, d: bytes):
        if len(c) != 4:
            raise Exception(f"Invalid command: {c}")
        await self.device_conns[did].send(c.encode() + d, text=False)

    async def device_run_prog(self, did: int, name: str):
        print(f'Running {name} on device {did}')
        await self.send_cmd(did, 'RUN ', name.encode() + b'\0' + cnc.prog_man.get_prog_contents(name).encode() + b'\0')

    async def device_send_key(self, did: int, key: int):
        print(f'Sending key {key} to {did}')
        await self.send_cmd(did, 'KEY ', key.to_bytes(4, byteorder='little'))

    async def send_prog_list_to_device(self, did: int, progs: list[str]):
        resp = bytearray()
        for prog in progs:
            resp += prog.encode() + b'\0'
        print(f'Sending progs list to device {did}')
        await self.send_cmd(did, 'PRGS', resp)

    async def broadcast(self, f, *args, **kwargs):
        fids = self.device_conns.keys()
        for fid in fids:
            try:
                await f(fid, *args, **kwargs)
            except Exception as e:
                print(f'Failed to send message to front {fid}: {e}')

    async def _handle_device_message(self, msg: bytes, device: ServerConnection, did: int):
        cmd = msg[:4].decode()
        data = msg[4:]
        match cmd:
            case 'RUN ':
                await self.device_run_prog(did, data.decode())
            case 'PRGS':
                await self.send_prog_list_to_device(did, cnc.prog_man.get_prog_list())
            case 'TSKS':
                res = []
                print('ddd', data.decode())
                for pair in data.decode().split('/'):
                    if pair == '':
                        continue
                    tid, name = pair.split(';')
                    res.append({'tid': tid, 'name': name})
                await cnc.front_man.broadcast(cnc.front_man.front_send_task_list, did, res)
            case _:
                print(f'Device {did} sent unknown command: {cmd}')


