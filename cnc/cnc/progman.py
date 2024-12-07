import os
from pathlib import Path
import cnc
# from cnc import dev_man


class ProgMan:
    progs_path = Path("programs")

    def notify_prog_list_change(self):
        new_list = self.get_prog_list()
        print(f'Sending new prog list: {new_list}')
        # dev_man.
        # TODO: actually notify

    def get_prog_list(self) -> list[str]:
        return os.listdir(self.progs_path)

    def get_prog_contents(self, name: str):
        return (self.progs_path / name).read_text()

    def del_prog(self, name: str):
        (self.progs_path / name).unlink()
        self.notify_prog_list_change()

    def set_prog(self, name: str, contents: str):
        (self.progs_path / name).write_text(contents)
        self.notify_prog_list_change()