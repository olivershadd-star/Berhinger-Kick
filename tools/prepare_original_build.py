#!/usr/bin/env python3
"""Stage the original project without rewriting its UI or musical behavior."""
import argparse
import hashlib
import json
import shutil
from pathlib import Path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--destination", type=Path, required=True)
    parser.add_argument("--serialport", type=Path, required=True)
    args = parser.parse_args()
    source, destination = args.source.resolve(), args.destination.resolve()
    if destination == source or source in destination.parents:
        parser.error("Staging must be outside the immutable upstream snapshot")
    if not (args.serialport / "juce_serialport.h").is_file():
        parser.error("Original serialport module absent; no replacement is generated")
    target = destination / "Kick"
    def digest_tree(root):
        return {f.relative_to(root).as_posix():hashlib.sha256(f.read_bytes()).hexdigest()
                for f in sorted(root.rglob("*")) if f.is_file()}
    recipe={"kick":digest_tree(source/"Kick"), "serialport":digest_tree(args.serialport)}
    stamp=destination/"staging-inputs.json"
    if target.exists():
        if stamp.is_file() and json.loads(stamp.read_text())==recipe:
            print("Reusing staged original project (same source inputs).")
            return
        parser.error("Staging inputs changed; use a fresh build directory")
    shutil.copytree(source / "Kick", target)
    serial = target / "modules/3rdParty/juce_serialport"
    if not serial.exists():
        shutil.copytree(args.serialport, serial)
    stamp.write_text(json.dumps(recipe,sort_keys=True)+"\n")
    print(f"Original Kick project staged: {target}")
    print("No musical source or interface was patched.")


if __name__ == "__main__":
    main()
