"""pip wrapper for the bundled Python that defaults --target to the ShapeWorks
per-user site-packages directory for `install`. Other pip subcommands pass
through unchanged (they read sys.path, which already includes the user-site).

Invoked via the `swpip` wrapper script — users just type
    swpip install <pkg>
and the package lands in the same versioned user-site directory that Studio,
the shapeworks CLI, and shapeworks.ensure_torch() all use.
"""

import os
import subprocess
import sys

from .torch_install import _user_site_packages


def main(args):
    if args and args[0] == "install":
        target = _user_site_packages()
        os.makedirs(target, exist_ok=True)
        # Insert --target right after the install subcommand. Any explicit
        # --target the user passed will be the *second* one and pip will
        # reject the duplicate, which is the right behavior — they should
        # drop our default if they want to override.
        args = ["install", "--target", target] + args[1:]
    return subprocess.call([sys.executable, "-m", "pip"] + args)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
