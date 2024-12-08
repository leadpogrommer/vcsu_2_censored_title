import os
from pathlib import Path
import cnc
# from cnc import dev_man
from asyncinotify import Inotify, Mask


class ProgMan:
    progs_path = Path("programs")

    async def notify_prog_list_change(self):
        new_list = self.get_prog_list()
        print(f'Sending new prog list: {new_list}')
        await cnc.front_man.broadcast(cnc.front_man.front_send_prog_list, self.get_prog_list())
        await cnc.dev_man.broadcast(cnc.dev_man.send_prog_list_to_device, self.get_prog_list())

    def get_prog_list(self) -> list[str]:
        return sorted(os.listdir(self.progs_path))

    def get_prog_contents(self, name: str):
        return (self.progs_path / name).read_text()

    # def del_prog(self, name: str):
    #     (self.progs_path / name).unlink()
    #     self.notify_prog_list_change()
    #
    # def set_prog(self, name: str, contents: str):
    #     (self.progs_path / name).write_text(contents)
    #     self.notify_prog_list_change()

    async def watch_files(self):
        with Inotify() as inotify:
            inotify.add_watch(self.progs_path, Mask.CREATE | Mask.DELETE)
            async for _ in inotify:
                print('Refreshing prog list')
                await self.notify_prog_list_change()
