# SPDX-License-Identifier: AGPL-3.0-or-later
"""Original synthetic demo content; no Behringer factory samples."""
from pathlib import Path
import math, random, wave, struct
root=Path(__file__).resolve().parents[1]/'demo-kit';root.mkdir(exist_ok=True)
names=['Kick','Snare','Closed-Hat','Open-Hat','Clap','Low-Tom','Mid-Tom','High-Tom','Rim','Shaker','Claves','Cowbell','Bass-C','Bass-E','Bass-G','Chime']
r=random.Random(700)
for i,name in enumerate(names):
 rate=48000; duration=[.6,.35,.13,.5,.3,.5,.4,.3,.12,.18,.18,.35,.65,.65,.65,.9][i]
 data=[];phase=0.;last=0.
 for n in range(int(rate*duration)):
  t=n/rate;noise=r.uniform(-1,1);high=noise-last;last=noise
  if i==0:
   phase+=2*math.pi*(48+110*math.exp(-t*40))/rate;s=math.sin(phase)*math.exp(-t*9)+.08*noise*math.exp(-t*100)
  elif i==1:s=(.6*noise*math.exp(-t*18)+.4*math.sin(2*math.pi*175*t)*math.exp(-t*25))
  elif i in (2,3):s=.24*high*math.exp(-t*(48 if i==2 else 10))
  elif i==4:s=noise*sum(.22*math.exp(-(t-d)*65) for d in (0,.013,.028) if t>=d)+.15*noise*math.exp(-t*12)
  elif i in (5,6,7):
   phase+=2*math.pi*([100,145,200][i-5]+50*math.exp(-t*30))/rate;s=.8*math.sin(phase)*math.exp(-t*12)
  elif i==8:s=.6*(math.sin(2*math.pi*810*t)+.4*math.sin(2*math.pi*1730*t))*math.exp(-t*65)
  elif i==9:s=.25*high*math.sin(min(1,t/.02)*math.pi/2)*math.exp(-t*28)
  elif i==10:s=.65*math.sin(2*math.pi*2200*t)*math.exp(-t*65)
  elif i==11:s=.3*(math.sin(2*math.pi*540*t)+math.sin(2*math.pi*800*t))*math.exp(-t*17)
  elif i in (12,13,14):
   f=[65.4064,82.4069,97.9989][i-12];s=(.75*math.sin(2*math.pi*f*t)+.18*math.sin(4*math.pi*f*t))*math.exp(-t*7)*min(1,t/.006)
  else:s=.35*(math.sin(2*math.pi*880*t)+.3*math.sin(2*math.pi*2217*t))*math.exp(-t*5)*min(1,t/.004)
  fade=min(1,n/32,(int(rate*duration)-n-1)/96);v=int(max(-.95,min(.95,s*.68*fade))*32767);data.append(struct.pack('<hh',v,v))
 with wave.open(str(root/f'{i+1:02d}-{name}.wav'),'wb') as f:f.setparams((2,2,rate,0,'NONE','not compressed'));f.writeframes(b''.join(data))
