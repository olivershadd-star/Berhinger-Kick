#!/usr/bin/env python3
"""Stage original KICK with traceable platform/build compatibility changes."""
import argparse, hashlib, json, re, shutil
from pathlib import Path
ROOT = Path(__file__).resolve().parents[1]
p = argparse.ArgumentParser(description=__doc__)
p.add_argument('--destination', type=Path, required=True)
a = p.parse_args()
dst = a.destination.resolve()
src = ROOT/'upstream/0700-AAM'
if dst == ROOT or dst == src or src in dst.parents:
    p.error('Staging must not overwrite the repository or immutable upstream sources')
def digest_tree(root):
    return {f.relative_to(root).as_posix():hashlib.sha256(f.read_bytes()).hexdigest()
            for f in sorted(root.rglob('*')) if f.is_file()}
recipe = {'upstream':digest_tree(src), 'modules':digest_tree(ROOT/'port/modules'),
          'patches':digest_tree(ROOT/'port/patches'),
          'script':hashlib.sha256(Path(__file__).read_bytes()).hexdigest()}
stamp = dst/'staging-inputs.json'
if dst.exists():
    if stamp.is_file() and json.loads(stamp.read_text()) == recipe:
        print(f'Reusing staged port: {dst}')
        raise SystemExit(0)
    p.error('Staging inputs changed or destination is not ours; choose a fresh build directory')
shutil.copytree(src, dst)
changes=[]
def edit(relative, old, new):
    f=dst/relative
    text=f.read_text()
    if old not in text: raise RuntimeError(f'Patch context absent: {relative}: {old[:80]}')
    f.write_text(text.replace(old,new))
    changes.append(str(relative))
shutil.copytree(ROOT/'port/modules', dst/'port_modules')
(dst/'AudIO/juce_modules.cmake').write_text('''juce_add_modules(${CMAKE_CURRENT_LIST_DIR}/AudIO
    ${CMAKE_CURRENT_LIST_DIR}/../port_modules/AudIO_constants
    ${CMAKE_CURRENT_LIST_DIR}/../port_modules/AudIO_juce)
''')
# JUCE module identifiers must match their directories. Many shipped headers
# accidentally declare ID common. Only metadata is corrected.
for f in (dst/'Kick/modules').rglob('*.h'):
    if f.stem != f.parent.name: continue
    t=f.read_text(errors='replace')
    if 'BEGIN_JUCE_MODULE_DECLARATION' not in t: continue
    fixed=re.sub(r'(\bID:\s*)\w+',lambda m:m[1]+f.stem,t,count=1)
    if fixed!=t: f.write_text(fixed);changes.append(str(f.relative_to(dst)))
# The absent serial module drives /dev/ttymxc0 only. Keep that Linux code,
# exclude its includes/types/callback from the Windows application.
edit('Kick/modules/3rdParty/CMakeLists.txt','    juce_serialport\n','')
edit('Kick/modules/input_events/input_events.h','signal, PktMgr, juce_serialport','signal, PktMgr, juce_events')
for file in ['Kick/modules/input_events/input_events.h','Kick/modules/input_events/input_events.cpp']:
    edit(file,'#include <juce_serialport/juce_serialport.h>','#if JUCE_LINUX\n#include <juce_serialport/juce_serialport.h>\n#endif')
edit('Kick/modules/input_events/input_events.cpp','            if( sPort ) sPort->close();','#if JUCE_LINUX\n            if( sPort ) sPort->close();\n#endif')
edit('Kick/modules/input_events/input_events.cpp','            int rxLength = (int)input->getTotalLength();','#if JUCE_LINUX\n            int rxLength = (int)input->getTotalLength();')
edit('Kick/modules/input_events/input_events.cpp','        void run() override','#endif\n        void run() override')
# Above guard must end inside the callback, before its closing brace.
f=dst/'Kick/modules/input_events/input_events.cpp'
t=f.read_text().replace('            }\n        }\n\n#endif\n        void run() override','            }\n#endif\n        }\n\n        void run() override')
f.write_text(t)
edit('Kick/modules/input_events/input_events.cpp','        std::unique_ptr<SerialPort>','#if JUCE_LINUX\n        std::unique_ptr<SerialPort>')
edit('Kick/modules/input_events/input_events.cpp','        moodycamel::ConcurrentQueue','#endif\n        moodycamel::ConcurrentQueue')
# Build the original in-process application only, not the Linux IPC services.
(dst/'Kick/apps/CMakeLists.txt').write_text('add_subdirectory(kick)\n')
changes.append('Kick/apps/CMakeLists.txt')
# C++ override compatibility in the published Airwindows adapter.
f=dst/'AudIO/AudIO/Objects/Plugins/Airwindows/audioeffectx.h'
if not f.exists():
    matches=list((dst/'AudIO').rglob('audioeffectx.h')); f=matches[0]
t=f.read_text(); fixed=t.replace('virtual std::vector<AudIO::ParamInfo>& getParameters() override', 'virtual const std::vector<AudIO::ParamInfo>& getParameters() override')
if fixed == t: raise RuntimeError('Airwindows compatibility patch context absent')
f.write_text(fixed);changes.append(str(f.relative_to(dst)))
# Core amalgamation uses std::this_thread without declaring <thread>.
f=dst/'AudIO/AudIO/AudIO.cpp';f.write_text('#include <thread>\n'+f.read_text());changes.append(str(f.relative_to(dst)))
# Additional exact-context compiler compatibility patches, kept reviewable.
for patch in json.loads((ROOT/'port/patches/compatibility.json').read_text()):
    edit(patch['file'], patch['old'], patch['new'])
(dst/'port-changes.json').write_text(json.dumps({'modified':sorted(set(changes)), 'adapter_source':'port/modules', 'purpose':'Windows build and device adaptation; original musical code retained'},indent=2))
stamp.write_text(json.dumps(recipe, sort_keys=True))
print(dst)
