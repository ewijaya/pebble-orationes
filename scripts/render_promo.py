#!/usr/bin/env python3
"""Render editable Chapel promotional vectors. Requires rsvg-convert; no app changes."""
import base64
from html import escape
from pathlib import Path
import subprocess

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'docs/images/promo'
IVORY='#F6F0E5'; BURGUNDY='#70283D'; GOLD='#B69A60'; INK='#252524'; MUTED='#655E56'
def text(x,y,value,size=24,color=INK,family='Helvetica Neue, Arial, sans-serif',weight='400',extra=''):
 return f'<text x="{x}" y="{y}" font-family="{family}" font-size="{size}" font-weight="{weight}" fill="{color}" {extra}>{escape(value)}</text>'
def mark(x,y,size):
 # Same broken O and cross construction as the existing social-preview SVG.
 return f'''<svg x="{x}" y="{y}" width="{size}" height="{size}" viewBox="0 0 360 360"><defs><mask id="m{x}{y}{size}"><rect width="360" height="360" fill="white"/><rect x="248" y="16" width="42" height="238" fill="black"/></mask></defs><circle cx="180" cy="207" r="128" fill="none" stroke="{INK}" stroke-width="39" mask="url(#m{x}{y}{size})"/><path d="M269 24V245M208 82H329" fill="none" stroke="{BURGUNDY}" stroke-width="30"/></svg>'''
def symbol(kind,x,y,color=BURGUNDY):
 paths={'bookmark':'M4 2H20V26L12 21 4 26Z','book':'M2 4Q7 1 12 5Q17 1 22 4V24Q17 21 12 25Q7 21 2 24ZM12 5V25','cross':'M12 2V26M3 10H21','sun':'M12 6V2M12 26V22M2 14H6M18 14H22M5 7L3 5M19 7L21 5M5 21L3 23M19 21L21 23M17 14A5 5 0 1 1 7 14A5 5 0 1 1 17 14','sliders':'M3 6H21M3 14H21M3 22H21M7 3V9M16 11V17M10 19V25'}
 return f'<g transform="translate({x} {y})" fill="none" stroke="{color}" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round"><path d="{paths[kind]}"/></g>'
def watch(screen,x,y,scale=1):
 uri='data:image/png;base64,'+base64.b64encode((ROOT/screen).read_bytes()).decode()
 return f'''<g transform="translate({x} {y}) scale({scale})"><rect x="70" y="0" width="128" height="462" rx="23" fill="#292C2B"/><rect x="4" y="84" width="10" height="42" rx="4" fill="#60635F"/><rect x="252" y="95" width="10" height="33" rx="4" fill="#60635F"/><rect x="252" y="149" width="10" height="33" rx="4" fill="#60635F"/><rect x="252" y="203" width="10" height="33" rx="4" fill="#60635F"/><rect x="12" y="49" width="240" height="354" rx="34" fill="#A3A6A0"/><rect x="16" y="53" width="232" height="346" rx="31" fill="#686C68"/><rect x="23" y="60" width="218" height="332" rx="27" fill="#171C1C"/><image x="32" y="111" width="200" height="228" href="{uri}" image-rendering="pixelated"/></g>'''
def render(name,w,h,body,dest=None):
 svg=f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="0 0 {w} {h}"><rect width="{w}" height="{h}" fill="{IVORY}"/>{body}</svg>'
 p=OUT/'source'/f'{name}.svg';p.parent.mkdir(parents=True,exist_ok=True);p.write_text(svg)
 if name == 'social-preview-1280x640':
  (ROOT/'docs/images/store/source/social-preview-1280x640.svg').write_text(svg)
 target=dest or OUT/f'{name}.png'
 subprocess.run(['rsvg-convert',str(p),'-o',str(target)],check=True)
 print(target.relative_to(ROOT))

def hero(w,h):
 # Layout is composed at 1200x630, with a fresh artboard for each ratio.
 s=min(w/1200,h/630);dx=(w-1200*s)/2;dy=(h-630*s)/2
 body=f'<rect x="{w*.635}" width="{w*.365}" height="{h}" fill="{BURGUNDY}"/>'
 inner='<path d="M794 552V269a164 164 0 0 1 328 0V552" fill="none" stroke="#A66D72" stroke-width="1"/>'
 inner+=mark(60,44,60)+text(137,86,'ORATIONES',25,weight='600',extra='letter-spacing="3"')
 inner+=text(62,245,'Make time',76,BURGUNDY,'Georgia, serif')+text(62,328,'for prayer.',76,BURGUNDY,'Georgia, serif')
 inner+=text(65,391,'A Catholic prayer book',27)+text(65,427,'for Pebble Time 2.',27)
 inner+=f'<path d="M65 467H120" stroke="{GOLD}" stroke-width="3"/>'
 inner+=text(65,507,'On your wrist. Available offline.',23,MUTED)
 inner+=text(65,568,'DISCOVER ORATIONES',16,BURGUNDY,weight='600',extra='letter-spacing="2"')
 inner+=watch('docs/images/promo/raw/preces-cyan.png',827,69,1.03)
 inner+=text(960,587,'LARGE TEXT · CLEAR CONTRAST',12,IVORY,weight='500',extra='text-anchor="middle" letter-spacing="1.5"')
 return body+f'<g transform="translate({dx} {dy}) scale({s})">{inner}</g>'
render('orationes-og-1200x630',1200,630,hero(1200,630))
render('social-preview-1280x640',1280,640,hero(1280,640),ROOT/'docs/images/store/social-preview-1280x640.png')
body=mark(28,25,40)+text(79,53,'ORATIONES',16,weight='600',extra='letter-spacing="2"')
body+=text(28,140,'Make time',46,BURGUNDY,'Georgia, serif')+text(28,192,'for prayer.',46,BURGUNDY,'Georgia, serif')
body+=text(30,242,'Catholic prayers for Pebble Time 2',17)+text(30,277,'LARGE TEXT  ·  CLEAR CONTRAST  ·  OFFLINE',10,BURGUNDY,weight='600',extra='letter-spacing="1"')
body+=f'<rect x="454" width="266" height="320" fill="{BURGUNDY}"/><path d="M480 303V127a108 108 0 0 1 216 0V303" fill="none" stroke="#A66D72"/>'
body+=watch('docs/images/promo/raw/preces-cyan.png',510,22,.61)
render('banner-720x320',720,320,body,ROOT/'docs/images/store/banner-720x320.png')
for size in (80,144):render(f'icon-{size}',size,size,mark(size*.12,size*.08,size*.76),ROOT/f'docs/images/store/icon-{size}.png')
render('icon-master',512,512,mark(61,41,390))
# Four standalone promotional cards: captions are outside untouched native screens.
for name,heading,sub,kind,screen in [
 ('01-read','A moment for prayer.','Large text. Clear contrast.','book','preces-cyan'),
 ('02-continue','Pick up where you paused.','Resume or start again.','bookmark','continue-cyan'),
 ('03-library','Find a prayer for today.','Browse All Prayers. Pin a favorite.','cross','all-prayers-cyan'),
 ('04-appearance','Read your way.','Light or dark. Large or Extra Large.','sliders','angelus-cyan')]:
 b=mark(42,35,40)+text(97,64,'ORATIONES',16,weight='600',extra='letter-spacing="2"')
 b+=symbol(kind,43,122)+text(83,145,heading,29,BURGUNDY,'Georgia, serif')+text(43,187,sub,17,MUTED)
 b+=f'<path d="M130 708V412a170 170 0 0 1 340 0V708" fill="none" stroke="#DFD2BA"/>'
 b+=watch(f'docs/images/promo/raw/{screen}.png',168,233,1)
 b+=text(300,761,'PEBBLE TIME 2  /  ORATIONES v0.7.0',12,MUTED,extra='text-anchor="middle" letter-spacing="1.2"')
 render(name,600,800,b)
# A reusable category legend for promotional layouts, never part of native UI.
b=text(40,59,'A prayer for each part of your day.',32,BURGUNDY,'Georgia, serif')
for i,(label,kind) in enumerate([('Daily Prayer','sun'),('Marian & Rosary','cross'),('Confession','book'),('Psalms & Other','book'),('Prayer Cards','bookmark')]):
 x=40+i*231;b+=symbol(kind,x,108)+text(x,163,label,19)
render('category-guide',1200,205,b)
