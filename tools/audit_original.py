#!/usr/bin/env python3
"""Inventory and integrity gate for the genuine KICK port. Never fills missing APIs."""
from __future__ import annotations
import argparse
import hashlib
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JUCE = {"juce_" + name for name in (
    "core", "events", "data_structures", "graphics", "gui_basics", "gui_extra",
    "audio_basics", "audio_devices", "audio_formats", "audio_processors", "audio_utils",
    "audio_plugin_client", "dsp", "opengl", "osc", "cryptography", "video",
    "analytics", "box2d", "product_unlocking", "midi_ci")}  # Framework modules, not serialport.


def evidence(root: Path, file: str, text: str) -> dict:
    path = root / file
    for number, line in enumerate(path.read_text(errors="replace").splitlines(), 1):
        if text in line:
            return {"path": file, "line": number, "text": line.strip()}
    raise ValueError(f"Evidence no longer present: {file}: {text}")


def inventory(source: Path, audio: Path, serial: Path) -> dict:
    issues = []
    checks = [
        ("audio-cmake", audio / "juce_modules.cmake", "AudIO/juce_modules.cmake absent",
         "Kick/3rdParty.cmake", "juce_modules.cmake"),
        ("audio-constants", None, "Module AudIO_constants absent",
         "Kick/modules/common/common.h", "#include <AudIO_constants/"),
        ("audio-juce", None, "Module AudIO_juce absent",
         "Kick/modules/audio_interface/audio_interface.h", "#include <AudIO_juce/"),
        ("serialport", serial / "juce_serialport.h", "Module juce_serialport absent",
         "Kick/modules/input_events/input_events.h", "#include <juce_serialport/"),
    ]
    for ident, path, message, file, match in checks:
        if ident == "audio-constants":
            present = any(audio.rglob("AudIO_constants.h"))
        elif ident == "audio-juce":
            present = any(audio.rglob("AudIO_juce.h"))
        else:
            present = path.is_file()
        if not present:
            issues.append({"id": ident, "severity": "blocker", "message": message,
                           "evidence": evidence(source, file, match)})
    modules = {}
    id_mismatches = []
    for directory in (source / "Kick/modules").rglob("*"):
        if not directory.is_dir():
            continue
        for ext in (".h", ".hpp"):
            header = directory / (directory.name + ext)
            if not header.is_file():
                continue
            text = header.read_text(errors="replace")
            match = re.search(r"BEGIN_JUCE_MODULE_DECLARATION(.*?)END_JUCE_MODULE_DECLARATION", text, re.S)
            if not match:
                continue
            body = match.group(1)
            deps = re.search(r"dependencies:\s*([^\n]+)", body)
            ident = re.search(r"\bID:\s*(\w+)", body)
            modules[directory.name] = {
                "path": header.relative_to(source).as_posix(),
                "dependencies": re.findall(r"[\w]+", deps.group(1)) if deps else []}
            if ident and ident.group(1) != directory.name:
                id_mismatches.append({"path": header.relative_to(source).as_posix(),
                                      "declared": ident.group(1), "directory": directory.name})
    # Some bundled third-party modules expose a header without a JUCE metadata block.
    for parent in (source / "Kick/modules", source / "Kick/modules/3rdParty", source / "Kick/modules/engine"):
        for directory in parent.iterdir():
            if not directory.is_dir() or directory.name in modules:
                continue
            for ext in (".h", ".hpp"):
                header = directory / (directory.name + ext)
                if header.is_file():
                    modules[directory.name] = {"path": header.relative_to(source).as_posix(), "dependencies": [], "metadata": "not-declared"}
                    break
    supplied = set(modules) | JUCE | {"AudIO", "AudIO_airwindows", "PktMgr"}
    if any(audio.rglob("AudIO_constants.h")):
        supplied.add("AudIO_constants")
    if any(audio.rglob("AudIO_juce.h")):
        supplied.add("AudIO_juce")
    if (serial / "juce_serialport.h").is_file():
        supplied.add("juce_serialport")
    unresolved = sorted({dep for module in modules.values() for dep in module["dependencies"]
                         if dep not in supplied})
    return {
        "status": "blocked" if issues or unresolved else "dependencies-present-not-built",
        "blockers": issues, "unresolved_module_dependencies": unresolved,
        "module_count": len(modules), "modules": modules,
        "module_id_mismatches_to_review": id_mismatches,
        "original_ui": evidence(source, "Kick/apps/kick/Main.cpp", "kick_ui::KickHome::Instance()"),
        "original_sequence": evidence(source, "Kick/modules/engine/audio_engine/engine/mt_audio_sequence.h", "int setLength( int Bars )"),
        "stale_simulator": evidence(source, "Kick/apps/kickem/simulator/kick_Simulator.cpp", "AppHome::Instance()"),
        "partial_contract_adapter": evidence(source, "Kick/apps/kick/kick_audio_interface.h", "return dummy;"),
        "note": "Inventory success is not a successful KICK build or functional validation."
    }


def verify_original(source: Path, provenance: Path) -> dict:
    expected = json.loads(provenance.read_text())["files"]
    differences = []
    for name, digest in expected.items():
        file = source / name
        if not file.is_file() or hashlib.sha256(file.read_bytes()).hexdigest() != digest:
            differences.append(name)
    actual = {f.relative_to(source).as_posix() for f in source.rglob("*") if f.is_file()}
    extra = sorted(actual - set(expected))
    return {"status": "pristine" if not differences and not extra else "modified",
            "checked_files": len(expected), "changed_or_missing": differences, "extra_files": extra}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=ROOT / "upstream/0700-AAM")
    parser.add_argument("--audio", type=Path)
    parser.add_argument("--serialport", type=Path)
    parser.add_argument("--json", type=Path)
    parser.add_argument("--strict", action="store_true", help="Fail if original dependencies are absent")
    parser.add_argument("--verify-original", action="store_true")
    args = parser.parse_args()
    source = args.source.resolve()
    report = inventory(source, (args.audio or source / "AudIO").resolve(),
                       (args.serialport or source / "Kick/modules/3rdParty/juce_serialport").resolve())
    if args.verify_original:
        report["source_integrity"] = verify_original(source, ROOT / "upstream/provenance.json")
    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"KICK source audit: {report['status']}; {report['module_count']} modules inventoried")
    for issue in report["blockers"]:
        e = issue["evidence"]
        print(f"  BLOCKER {issue['id']}: {issue['message']} ({e['path']}:{e['line']})")
    if report["unresolved_module_dependencies"]:
        print("  Unresolved modules: " + ", ".join(report["unresolved_module_dependencies"]))
    if args.verify_original:
        integrity = report["source_integrity"]
        print(f"  Upstream integrity: {integrity['status']} ({integrity['checked_files']} files)")
        if integrity["status"] != "pristine":
            return 3
    return 2 if args.strict and report["status"] == "blocked" else 0


if __name__ == "__main__":
    raise SystemExit(main())
