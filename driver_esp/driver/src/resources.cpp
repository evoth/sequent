#include "resources.h"

const char indexHtml[] PROGMEM = R"rawliteral(
<!doctype html>
<html lang="en">
  <head>
    <meta charset="UTF-8" />
    <link rel="icon" type="image/svg+xml" href="/icon.svg" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>ESP Sequent Driver</title>
    <script type="module">var it=Object.defineProperty;var ot=(t,e,n)=>e in t?it(t,e,{enumerable:!0,configurable:!0,writable:!0,value:n}):t[e]=n;var $e=(t,e,n)=>(ot(t,typeof e!="symbol"?e+"":e,n),n);(function(){const e=document.createElement("link").relList;if(e&&e.supports&&e.supports("modulepreload"))return;for(const s of document.querySelectorAll('link[rel="modulepreload"]'))l(s);new MutationObserver(s=>{for(const i of s)if(i.type==="childList")for(const r of i.addedNodes)r.tagName==="LINK"&&r.rel==="modulepreload"&&l(r)}).observe(document,{childList:!0,subtree:!0});function n(s){const i={};return s.integrity&&(i.integrity=s.integrity),s.referrerPolicy&&(i.referrerPolicy=s.referrerPolicy),s.crossOrigin==="use-credentials"?i.credentials="include":s.crossOrigin==="anonymous"?i.credentials="omit":i.credentials="same-origin",i}function l(s){if(s.ep)return;s.ep=!0;const i=n(s);fetch(s.href,i)}})();function E(){}const rt=t=>t;function ut(t,e){for(const n in e)t[n]=e[n];return t}function Ge(t){return t()}function Ie(){return Object.create(null)}function X(t){t.forEach(Ge)}function ye(t){return typeof t=="function"}function B(t,e){return t!=t?e==e:t!==e||t&&typeof t=="object"||typeof t=="function"}function ct(t){return Object.keys(t).length===0}function ft(t,...e){if(t==null){for(const l of e)l(void 0);return E}const n=t.subscribe(...e);return n.unsubscribe?()=>n.unsubscribe():n}function L(t,e,n){t.$$.on_destroy.push(ft(e,n))}function Qe(t,e,n,l){if(t){const s=Xe(t,e,n,l);return t[0](s)}}function Xe(t,e,n,l){return t[1]&&l?ut(n.ctx.slice(),t[1](l(e))):n.ctx}function Ye(t,e,n,l){if(t[2]&&l){const s=t[2](l(n));if(e.dirty===void 0)return s;if(typeof s=="object"){const i=[],r=Math.max(e.dirty.length,s.length);for(let o=0;o<r;o+=1)i[o]=e.dirty[o]|s[o];return i}return e.dirty|s}return e.dirty}function Ze(t,e,n,l,s,i){if(s){const r=Xe(e,n,l,i);t.p(r,s)}}function xe(t){if(t.ctx.length>32){const e=[],n=t.ctx.length/32;for(let l=0;l<n;l++)e[l]=-1;return e}return-1}function Ne(t){return t??""}function J(t,e,n){return t.set(n),e}const et=typeof window<"u";let at=et?()=>window.performance.now():()=>Date.now(),Se=et?t=>requestAnimationFrame(t):E;const te=new Set;function tt(t){te.forEach(e=>{e.c(t)||(te.delete(e),e.f())}),te.size!==0&&Se(tt)}function dt(t){let e;return te.size===0&&Se(tt),{promise:new Promise(n=>{te.add(e={c:t,f:n})}),abort(){te.delete(e)}}}function b(t,e){t.appendChild(e)}function nt(t){if(!t)return document;const e=t.getRootNode?t.getRootNode():t.ownerDocument;return e&&e.host?e:t.ownerDocument}function pt(t){const e=w("style");return e.textContent="/* empty */",_t(nt(t),e),e.sheet}function _t(t,e){return b(t.head||t,e),e.sheet}function k(t,e,n){t.insertBefore(e,n||null)}function v(t){t.parentNode&&t.parentNode.removeChild(t)}function Ce(t,e){for(let n=0;n<t.length;n+=1)t[n]&&t[n].d(e)}function w(t){return document.createElement(t)}function R(t){return document.createElementNS("http://www.w3.org/2000/svg",t)}function I(t){return document.createTextNode(t)}function C(){return I(" ")}function he(){return I("")}function j(t,e,n,l){return t.addEventListener(e,n,l),()=>t.removeEventListener(e,n,l)}function f(t,e,n){n==null?t.removeAttribute(e):t.getAttribute(e)!==n&&t.setAttribute(e,n)}function mt(t){return Array.from(t.childNodes)}function Q(t,e){e=""+e,t.data!==e&&(t.data=e)}function Oe(t,e){t.value=e??""}function ht(t,e,n,l){n==null?t.style.removeProperty(e):t.style.setProperty(e,n,l?"important":"")}function gt(t,e,{bubbles:n=!1,cancelable:l=!1}={}){return new CustomEvent(t,{detail:e,bubbles:n,cancelable:l})}const de=new Map;let pe=0;function $t(t){let e=5381,n=t.length;for(;n--;)e=(e<<5)-e^t.charCodeAt(n);return e>>>0}function bt(t,e){const n={stylesheet:pt(e),rules:{}};return de.set(t,n),n}function Le(t,e,n,l,s,i,r,o=0){const u=16.666/l;let c=`{
`;for(let p=0;p<=1;p+=u){const g=e+(n-e)*i(p);c+=p*100+`%{${r(g,1-g)}}
`}const a=c+`100% {${r(n,1-n)}}
}`,d=`__svelte_${$t(a)}_${o}`,m=nt(t),{stylesheet:_,rules:h}=de.get(m)||bt(m,t);h[d]||(h[d]=!0,_.insertRule(`@keyframes ${d} ${a}`,_.cssRules.length));const $=t.style.animation||"";return t.style.animation=`${$?`${$}, `:""}${d} ${l}ms linear ${s}ms 1 both`,pe+=1,d}function wt(t,e){const n=(t.style.animation||"").split(", "),l=n.filter(e?i=>i.indexOf(e)<0:i=>i.indexOf("__svelte")===-1),s=n.length-l.length;s&&(t.style.animation=l.join(", "),pe-=s,pe||vt())}function vt(){Se(()=>{pe||(de.forEach(t=>{const{ownerNode:e}=t.stylesheet;e&&v(e)}),de.clear())})}let Ee;function ie(t){Ee=t}const x=[],ve=[];let ne=[];const je=[],kt=Promise.resolve();let ke=!1;function yt(){ke||(ke=!0,kt.then(lt))}function oe(t){ne.push(t)}const be=new Set;let Y=0;function lt(){if(Y!==0)return;const t=Ee;do{try{for(;Y<x.length;){const e=x[Y];Y++,ie(e),St(e.$$)}}catch(e){throw x.length=0,Y=0,e}for(ie(null),x.length=0,Y=0;ve.length;)ve.pop()();for(let e=0;e<ne.length;e+=1){const n=ne[e];be.has(n)||(be.add(n),n())}ne.length=0}while(x.length);for(;je.length;)je.pop()();ke=!1,be.clear(),ie(t)}function St(t){if(t.fragment!==null){t.update(),X(t.before_update);const e=t.dirty;t.dirty=[-1],t.fragment&&t.fragment.p(t.ctx,e),t.after_update.forEach(oe)}}function Ct(t){const e=[],n=[];ne.forEach(l=>t.indexOf(l)===-1?e.push(l):n.push(l)),n.forEach(l=>l()),ne=e}let se;function Et(){return se||(se=Promise.resolve(),se.then(()=>{se=null})),se}function we(t,e,n){t.dispatchEvent(gt(`${e?"intro":"outro"}${n}`))}const fe=new Set;let T;function qe(){T={r:0,c:[],p:T}}function Pe(){T.r||X(T.c),T=T.p}function S(t,e){t&&t.i&&(fe.delete(t),t.i(e))}function P(t,e,n,l){if(t&&t.o){if(fe.has(t))return;fe.add(t),T.c.push(()=>{fe.delete(t),l&&(n&&t.d(1),l())}),t.o(e)}else l&&l()}const qt={duration:0};function Re(t,e,n,l){let i=e(t,n,{direction:"both"}),r=l?0:1,o=null,u=null,c=null,a;function d(){c&&wt(t,c)}function m(h,$){const p=h.b-r;return $*=Math.abs(p),{a:r,b:h.b,d:p,duration:$,start:h.start,end:h.start+$,group:h.group}}function _(h){const{delay:$=0,duration:p=300,easing:g=rt,tick:q=E,css:U}=i||qt,W={start:at()+$,b:h};h||(W.group=T,T.r+=1),"inert"in t&&(h?a!==void 0&&(t.inert=a):(a=t.inert,t.inert=!0)),o||u?u=W:(U&&(d(),c=Le(t,r,h,p,$,g,U)),h&&q(0,1),o=m(W,p),oe(()=>we(t,h,"start")),dt(y=>{if(u&&y>u.start&&(o=m(u,p),u=null,we(t,o.b,"start"),U&&(d(),c=Le(t,r,o.b,o.duration,0,g,i.css))),o){if(y>=o.end)q(r=o.b,1-r),we(t,o.b,"end"),u||(o.b?d():--o.group.r||X(o.group.c)),o=null;else if(y>=o.start){const N=y-o.start;r=o.a+o.d*g(N/o.duration),q(r,1-r)}}return!!(o||u)}))}return{run(h){ye(i)?Et().then(()=>{i=i({direction:h?"in":"out"}),_(h)}):_(h)},end(){d(),o=u=null}}}function le(t){return(t==null?void 0:t.length)!==void 0?t:Array.from(t)}function F(t){t&&t.c()}function M(t,e,n){const{fragment:l,after_update:s}=t.$$;l&&l.m(e,n),oe(()=>{const i=t.$$.on_mount.map(Ge).filter(ye);t.$$.on_destroy?t.$$.on_destroy.push(...i):X(i),t.$$.on_mount=[]}),s.forEach(oe)}function A(t,e){const n=t.$$;n.fragment!==null&&(Ct(n.after_update),X(n.on_destroy),n.fragment&&n.fragment.d(e),n.on_destroy=n.fragment=null,n.ctx=[])}function Pt(t,e){t.$$.dirty[0]===-1&&(x.push(t),yt(),t.$$.dirty.fill(0)),t.$$.dirty[e/31|0]|=1<<e%31}function K(t,e,n,l,s,i,r=null,o=[-1]){const u=Ee;ie(t);const c=t.$$={fragment:null,ctx:[],props:i,update:E,not_equal:s,bound:Ie(),on_mount:[],on_destroy:[],on_disconnect:[],before_update:[],after_update:[],context:new Map(e.context||(u?u.$$.context:[])),callbacks:Ie(),dirty:o,skip_bound:!1,root:e.target||u.$$.root};r&&r(c.root);let a=!1;if(c.ctx=n?n(t,e.props||{},(d,m,..._)=>{const h=_.length?_[0]:m;return c.ctx&&s(c.ctx[d],c.ctx[d]=h)&&(!c.skip_bound&&c.bound[d]&&c.bound[d](h),a&&Pt(t,d)),m}):[],c.update(),a=!0,X(c.before_update),c.fragment=l?l(c.ctx):!1,e.target){if(e.hydrate){const d=mt(e.target);c.fragment&&c.fragment.l(d),d.forEach(v)}else c.fragment&&c.fragment.c();e.intro&&S(t.$$.fragment),M(t,e.target,e.anchor),lt()}ie(u)}class V{constructor(){$e(this,"$$");$e(this,"$$set")}$destroy(){A(this,1),this.$destroy=E}$on(e,n){if(!ye(n))return E;const l=this.$$.callbacks[e]||(this.$$.callbacks[e]=[]);return l.push(n),()=>{const s=l.indexOf(n);s!==-1&&l.splice(s,1)}}$set(e){this.$$set&&!ct(e)&&(this.$$.skip_bound=!0,this.$$set(e),this.$$.skip_bound=!1)}}const It="4";typeof window<"u"&&(window.__svelte||(window.__svelte={v:new Set})).v.add(It);const Z=[];function ue(t,e=E){let n;const l=new Set;function s(o){if(B(t,o)&&(t=o,n)){const u=!Z.length;for(const c of l)c[1](),Z.push(c,t);if(u){for(let c=0;c<Z.length;c+=2)Z[c][0](Z[c+1]);Z.length=0}}}function i(o){s(o(t))}function r(o,u=E){const c=[o,u];return l.add(c),l.size===1&&(n=e(s,i)||E),o(t),()=>{l.delete(c),l.size===0&&n&&(n(),n=null)}}return{set:s,update:i,subscribe:r}}const ae=ue(!1),_e=ue("192.168.4.1"),re=ue({states:[],sequenceKey:{},cameras:[],cameraIP:"192.168.4.7"}),ee=ue(!1),me=ue();function Nt(t){const e=t-1;return e*e*e+1}function Me(t,{delay:e=0,duration:n=400,easing:l=Nt,axis:s="y"}={}){const i=getComputedStyle(t),r=+i.opacity,o=s==="y"?"height":"width",u=parseFloat(i[o]),c=s==="y"?["top","bottom"]:["left","right"],a=c.map(g=>`${g[0].toUpperCase()}${g.slice(1)}`),d=parseFloat(i[`padding${a[0]}`]),m=parseFloat(i[`padding${a[1]}`]),_=parseFloat(i[`margin${a[0]}`]),h=parseFloat(i[`margin${a[1]}`]),$=parseFloat(i[`border${a[0]}Width`]),p=parseFloat(i[`border${a[1]}Width`]);return{delay:e,duration:n,easing:l,css:g=>`overflow: hidden;opacity: ${Math.min(g*20,1)*r};${o}: ${g*u}px;padding-${c[0]}: ${g*d}px;padding-${c[1]}: ${g*m}px;margin-${c[0]}: ${g*_}px;margin-${c[1]}: ${g*h}px;border-${c[0]}-width: ${g*$}px;border-${c[1]}-width: ${g*p}px;`}}const Ot=t=>({}),Ae=t=>({});function De(t){let e,n,l,s;const i=t[4].default,r=Qe(i,t,t[3],null);return{c(){e=w("div"),r&&r.c(),f(e,"id",n=`${t[0]}-body`),f(e,"class","body svelte-sext2k")},m(o,u){k(o,e,u),r&&r.m(e,null),s=!0},p(o,u){r&&r.p&&(!s||u&8)&&Ze(r,i,o,o[3],s?Ye(i,o[3],u,null):xe(o[3]),null),(!s||u&1&&n!==(n=`${o[0]}-body`))&&f(e,"id",n)},i(o){s||(S(r,o),o&&oe(()=>{s&&(l||(l=Re(e,Me,{duration:300},!0)),l.run(1))}),s=!0)},o(o){P(r,o),o&&(l||(l=Re(e,Me,{duration:300},!1)),l.run(0)),s=!1},d(o){o&&v(e),r&&r.d(o),o&&l&&l.end()}}}function Lt(t){let e,n,l,s,i,r,o,u,c,a,d,m,_;const h=t[4].heading,$=Qe(h,t,t[3],Ae);let p=t[1]&&De(t);return{c(){e=w("div"),n=w("button"),$&&$.c(),l=C(),s=w("div"),i=R("svg"),r=R("path"),a=C(),p&&p.c(),f(r,"d","M15 18l-6-6 6-6"),f(i,"class",o=Ne(t[1]?"rotated":"")+" svelte-sext2k"),f(i,"xmlns","http://www.w3.org/2000/svg"),f(i,"width","24"),f(i,"height","24"),f(i,"viewBox","0 0 24 24"),f(i,"fill","none"),f(i,"stroke","currentColor"),f(i,"stroke-width","2"),f(i,"stroke-linecap","round"),f(i,"stroke-linejoin","round"),f(s,"class","arrow-container svelte-sext2k"),f(n,"class","heading svelte-sext2k"),f(n,"title",u=t[1]?"Collapse section":"Expand section"),f(n,"aria-expanded",t[1]),f(n,"aria-controls",c=`${t[0]}-body`),f(e,"class","section svelte-sext2k")},m(g,q){k(g,e,q),b(e,n),$&&$.m(n,null),b(n,l),b(n,s),b(s,i),b(i,r),b(e,a),p&&p.m(e,null),d=!0,m||(_=j(n,"click",t[2]),m=!0)},p(g,[q]){$&&$.p&&(!d||q&8)&&Ze($,h,g,g[3],d?Ye(h,g[3],q,Ot):xe(g[3]),Ae),(!d||q&2&&o!==(o=Ne(g[1]?"rotated":"")+" svelte-sext2k"))&&f(i,"class",o),(!d||q&2&&u!==(u=g[1]?"Collapse section":"Expand section"))&&f(n,"title",u),(!d||q&2)&&f(n,"aria-expanded",g[1]),(!d||q&1&&c!==(c=`${g[0]}-body`))&&f(n,"aria-controls",c),g[1]?p?(p.p(g,q),q&2&&S(p,1)):(p=De(g),p.c(),S(p,1),p.m(e,null)):p&&(qe(),P(p,1,1,()=>{p=null}),Pe())},i(g){d||(S($,g),S(p),d=!0)},o(g){P($,g),P(p),d=!1},d(g){g&&v(e),$&&$.d(g),p&&p.d(),m=!1,_()}}}function jt(t,e,n){let{$$slots:l={},$$scope:s}=e,{name:i}=e;const r=`${i}-expanded`;let o;localStorage.getItem(r)?o=localStorage.getItem(r)==String(!0):o=!0;function u(){n(1,o=!o),i&&localStorage.setItem(r,String(o))}return t.$$set=c=>{"name"in c&&n(0,i=c.name),"$$scope"in c&&n(3,s=c.$$scope)},[i,o,u,s,l]}class ge extends V{constructor(e){super(),K(this,e,jt,Lt,B,{name:0})}}function Rt(t){let e,n,l;return{c(){e=w("button"),e.textContent="Disconnect"},m(s,i){k(s,e,i),n||(l=j(e,"click",t[5]),n=!0)},p:E,d(s){s&&v(e),n=!1,l()}}}function Mt(t){let e,n,l;return{c(){e=w("button"),e.textContent="Connect"},m(s,i){k(s,e,i),n||(l=j(e,"click",t[4]),n=!0)},p:E,d(s){s&&v(e),n=!1,l()}}}function At(t){let e;return{c(){e=w("p"),e.textContent="Disconnected",f(e,"class","error")},m(n,l){k(n,e,l)},p:E,d(n){n&&v(e)}}}function Dt(t){let e,n,l;return{c(){e=w("p"),n=I("Connected to "),l=I(t[3]),f(e,"class","success")},m(s,i){k(s,e,i),b(e,n),b(e,l)},p(s,i){i&8&&Q(l,s[3])},d(s){s&&v(e)}}}function Ft(t){let e;return{c(){e=w("p"),e.textContent="Connecting..."},m(n,l){k(n,e,l)},p:E,d(n){n&&v(e)}}}function Tt(t){let e,n,l,s,i,r,o,u,c;function a(p,g){return!p[2]||p[0]!=p[3]?Mt:Rt}let d=a(t),m=d(t);function _(p,g){return p[1]?Ft:p[2]?Dt:At}let h=_(t),$=h(t);return{c(){e=w("label"),n=w("span"),n.textContent="IP address:",l=C(),s=w("input"),i=C(),m.c(),r=C(),$.c(),o=he(),f(s,"placeholder","ESP IP address")},m(p,g){k(p,e,g),b(e,n),b(e,l),b(e,s),Oe(s,t[0]),k(p,i,g),m.m(p,g),k(p,r,g),$.m(p,g),k(p,o,g),u||(c=j(s,"input",t[6]),u=!0)},p(p,g){g&1&&s.value!==p[0]&&Oe(s,p[0]),d===(d=a(p))&&m?m.p(p,g):(m.d(1),m=d(p),m&&(m.c(),m.m(r.parentNode,r))),h===(h=_(p))&&$?$.p(p,g):($.d(1),$=h(p),$&&($.c(),$.m(o.parentNode,o)))},d(p){p&&(v(e),v(i),v(r),v(o)),m.d(p),$.d(p),u=!1,c()}}}function Bt(t){let e;return{c(){e=w("h2"),e.textContent="Connect to ESP",f(e,"slot","heading")},m(n,l){k(n,e,l)},p:E,d(n){n&&v(e)}}}function Ut(t){let e,n;return e=new ge({props:{name:"connect-esp",$$slots:{heading:[Bt],default:[Tt]},$$scope:{ctx:t}}}),{c(){F(e.$$.fragment)},m(l,s){M(e,l,s),n=!0},p(l,[s]){const i={};s&1039&&(i.$$scope={dirty:s,ctx:l}),e.$set(i)},i(l){n||(S(e.$$.fragment,l),n=!0)},o(l){P(e.$$.fragment,l),n=!1},d(l){A(e,l)}}}function zt(t,e,n){let l,s,i,r,o;L(t,me,_=>n(7,l=_)),L(t,ae,_=>n(2,s=_)),L(t,ee,_=>n(8,i=_)),L(t,re,_=>n(9,r=_)),L(t,_e,_=>n(3,o=_));let u=o,c;function a(){J(_e,o=u,o),l==null||l.close(),J(me,l=new WebSocket(`ws://${o}:81`),l),l.addEventListener("message",_=>{J(re,r=JSON.parse(_.data),r),console.log(r),J(ee,i=!1,i)}),l.addEventListener("open",_=>{J(ae,s=!0,s),n(1,c=!1)}),l.addEventListener("close",_=>{J(ae,s=!1,s),n(1,c=!1)}),n(1,c=!0)}function d(){l==null||l.close()}a();function m(){u=this.value,n(0,u)}return[u,c,s,o,a,d,m]}class Ht extends V{constructor(e){super(),K(this,e,zt,Ut,B,{})}}function Fe(t,e,n){const l=t.slice();return l[18]=e[n],l}function Jt(t){let e,n,l,s,i,r;function o(){return t[9](t[18])}return{c(){e=w("button"),n=R("svg"),l=R("polygon"),f(l,"points","5 3 19 12 5 21 5 3"),f(n,"xmlns","http://www.w3.org/2000/svg"),f(n,"width","1em"),f(n,"height","1em"),f(n,"viewBox","0 0 24 24"),f(n,"fill","none"),f(n,"stroke","currentColor"),f(n,"stroke-width","2"),f(n,"stroke-linecap","round"),f(n,"stroke-linejoin","round"),f(e,"title",s=`Start '${t[18]}' sequence`)},m(u,c){k(u,e,c),b(e,n),b(n,l),i||(r=j(e,"click",o),i=!0)},p(u,c){t=u,c&4&&s!==(s=`Start '${t[18]}' sequence`)&&f(e,"title",s)},d(u){u&&v(e),i=!1,r()}}}function Kt(t){let e,n,l,s,i,r;function o(){return t[8](t[18])}return{c(){e=w("button"),n=R("svg"),l=R("rect"),f(l,"x","3"),f(l,"y","3"),f(l,"width","18"),f(l,"height","18"),f(l,"rx","2"),f(l,"ry","2"),f(n,"xmlns","http://www.w3.org/2000/svg"),f(n,"width","1em"),f(n,"height","1em"),f(n,"viewBox","0 0 24 24"),f(n,"fill","none"),f(n,"stroke","currentColor"),f(n,"stroke-width","2"),f(n,"stroke-linecap","round"),f(n,"stroke-linejoin","round"),f(e,"title",s=`Stop '${t[18]}' sequence`)},m(u,c){k(u,e,c),b(e,n),b(n,l),i||(r=j(e,"click",o),i=!0)},p(u,c){t=u,c&4&&s!==(s=`Stop '${t[18]}' sequence`)&&f(e,"title",s)},d(u){u&&v(e),i=!1,r()}}}function Te(t){let e,n;function l(o,u){return o[0]===o[18]?Gt:o[3][o[18]].isRunning?Wt:Vt}let s=l(t),i=s(t),r=t[3][t[18]].isRunning&&Be(t);return{c(){i.c(),e=C(),r&&r.c(),n=he()},m(o,u){i.m(o,u),k(o,e,u),r&&r.m(o,u),k(o,n,u)},p(o,u){s!==(s=l(o))&&(i.d(1),i=s(o),i&&(i.c(),i.m(e.parentNode,e))),o[3][o[18]].isRunning?r?r.p(o,u):(r=Be(o),r.c(),r.m(n.parentNode,n)):r&&(r.d(1),r=null)},d(o){o&&(v(e),v(n)),i.d(o),r&&r.d(o)}}}function Vt(t){let e;return{c(){e=w("p"),e.textContent="Sequence stopped."},m(n,l){k(n,e,l)},d(n){n&&v(e)}}}function Wt(t){let e;return{c(){e=w("p"),e.textContent="Sequence running..."},m(n,l){k(n,e,l)},d(n){n&&v(e)}}}function Gt(t){let e;return{c(){e=w("p"),e.textContent="Loading..."},m(n,l){k(n,e,l)},d(n){n&&v(e)}}}function Be(t){let e,n,l=t[3][t[18]].actionIndex+"",s,i,r=t[3][t[18]].totalActions+"",o,u,c=t[1][t[18]]+"",a;return{c(){e=w("p"),n=I("Action index: "),s=I(l),i=I(" / "),o=I(r),u=I(`\r
              Time until next action: `),a=I(c)},m(d,m){k(d,e,m),b(e,n),b(e,s),b(e,i),b(e,o),b(e,u),b(e,a)},p(d,m){m&12&&l!==(l=d[3][d[18]].actionIndex+"")&&Q(s,l),m&12&&r!==(r=d[3][d[18]].totalActions+"")&&Q(o,r),m&6&&c!==(c=d[1][d[18]]+"")&&Q(a,c)},d(d){d&&v(e)}}}function Ue(t){let e,n,l,s=t[18]+"",i,r,o,u,c,a,d,m,_,h,$,p,g,q,U=Object.keys(t[3][t[18]]).length>0,W,y,N;function G(z,H){return z[3][z[18]].isRunning?Kt:Jt}let ce=G(t),D=ce(t);function st(){return t[10](t[18])}let O=U&&Te(t);return{c(){e=w("div"),n=w("div"),l=w("div"),i=I(s),r=C(),o=w("div"),u=C(),D.c(),c=C(),a=w("button"),d=R("svg"),m=R("polyline"),_=R("path"),h=R("line"),$=R("line"),g=C(),q=w("div"),O&&O.c(),W=C(),ht(o,"flex","1"),f(m,"points","3 6 5 6 21 6"),f(_,"d","M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"),f(h,"x1","10"),f(h,"y1","11"),f(h,"x2","10"),f(h,"y2","17"),f($,"x1","14"),f($,"y1","11"),f($,"x2","14"),f($,"y2","17"),f(d,"xmlns","http://www.w3.org/2000/svg"),f(d,"width","1em"),f(d,"height","1em"),f(d,"viewBox","0 0 24 24"),f(d,"fill","none"),f(d,"stroke","currentColor"),f(d,"stroke-width","2"),f(d,"stroke-linecap","round"),f(d,"stroke-linejoin","round"),f(a,"title",p=`Delete '${t[18]}' sequence`),f(n,"class","sequence-container-inner svelte-104po3j"),f(q,"class","status svelte-104po3j"),f(e,"class","sequence-container svelte-104po3j")},m(z,H){k(z,e,H),b(e,n),b(n,l),b(l,i),b(n,r),b(n,o),b(n,u),D.m(n,null),b(n,c),b(n,a),b(a,d),b(d,m),b(d,_),b(d,h),b(d,$),b(e,g),b(e,q),O&&O.m(q,null),b(e,W),y||(N=j(a,"click",st),y=!0)},p(z,H){t=z,H&4&&s!==(s=t[18]+"")&&Q(i,s),ce===(ce=G(t))&&D?D.p(t,H):(D.d(1),D=ce(t),D&&(D.c(),D.m(n,c))),H&4&&p!==(p=`Delete '${t[18]}' sequence`)&&f(a,"title",p),H&12&&(U=Object.keys(t[3][t[18]]).length>0),U?O?O.p(t,H):(O=Te(t),O.c(),O.m(q,null)):O&&(O.d(1),O=null)},d(z){z&&v(e),D.d(),O&&O.d(),y=!1,N()}}}function Qt(t){let e,n,l,s,i,r=le(t[2]),o=[];for(let u=0;u<r.length;u+=1)o[u]=Ue(Fe(t,r,u));return{c(){e=w("button"),e.innerHTML='<svg xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"><path d="M21.5 2v6h-6M21.34 15.57a10 10 0 1 1-.57-8.38"></path></svg>',n=C();for(let u=0;u<o.length;u+=1)o[u].c();l=he(),f(e,"title","Refresh sequence list")},m(u,c){k(u,e,c),k(u,n,c);for(let a=0;a<o.length;a+=1)o[a]&&o[a].m(u,c);k(u,l,c),s||(i=j(e,"click",t[6]),s=!0)},p(u,c){if(c&191){r=le(u[2]);let a;for(a=0;a<r.length;a+=1){const d=Fe(u,r,a);o[a]?o[a].p(d,c):(o[a]=Ue(d),o[a].c(),o[a].m(l.parentNode,l))}for(;a<o.length;a+=1)o[a].d(1);o.length=r.length}},d(u){u&&(v(e),v(n),v(l)),Ce(o,u),s=!1,i()}}}function Xt(t){let e;return{c(){e=w("h2"),e.textContent="Sequences",f(e,"slot","heading")},m(n,l){k(n,e,l)},p:E,d(n){n&&v(e)}}}function Yt(t){let e,n;return e=new ge({props:{name:"sequences",$$slots:{heading:[Xt],default:[Qt]},$$scope:{ctx:t}}}),{c(){F(e.$$.fragment)},m(l,s){M(e,l,s),n=!0},p(l,[s]){const i={};s&2097167&&(i.$$scope={dirty:s,ctx:l}),e.$set(i)},i(l){n||(S(e.$$.fragment,l),n=!0)},o(l){P(e.$$.fragment,l),n=!1},d(l){A(e,l)}}}function Zt(t,e,n){let l,s,i,r;L(t,re,y=>n(13,l=y)),L(t,_e,y=>n(14,s=y)),L(t,ee,y=>n(15,i=y)),L(t,me,y=>n(16,r=y));let o="",u={},c={},a={},d=[],m={};$(),re.subscribe(y=>{n(3,m=g(d));for(const[N,G]of Object.entries(m))u[N]=Date.now(),clearInterval(a[N]),G.isRunning?a[N]=setInterval(()=>{n(1,c[N]=((G.timeUntilNext-(Date.now()-u[N]))/1e3).toFixed(1),c)},100):n(1,c[N]="0",c)}),ee.subscribe(y=>n(0,o=y&&o!==""?o:""));function _(y){r.send(JSON.stringify({command:"start",filename:y})),J(ee,i=!0,i),n(0,o=y)}function h(y){r.send(JSON.stringify({command:"stop",filename:y})),J(ee,i=!0,i),n(0,o=y)}async function $(){const y=await fetch(`http://${s}/seq-files`);n(2,d=(await y.json()).files),n(3,m=g(d))}async function p(y){await fetch(`http://${s}/delete?file=`+encodeURIComponent(y),{method:"DELETE"}),$()}function g(y){return Object.fromEntries(y.map(N=>[N,l.states.find(G=>G.filePath===N)??{}]))}return[o,c,d,m,_,h,$,p,y=>h(y),y=>_(y),y=>p(y)]}class xt extends V{constructor(e){super(),K(this,e,Zt,Yt,B,{})}}function ze(t){let e,n;return{c(){e=w("input"),f(e,"class","button"),f(e,"type","submit"),e.value=n=`Upload '${t[0].files[0].name}'`},m(l,s){k(l,e,s)},p(l,s){s&1&&n!==(n=`Upload '${l[0].files[0].name}'`)&&(e.value=n)},d(l){l&&v(e)}}}function en(t){let e,n,l,s,i,r,o,u,c,a,d,m,_=t[0]&&t[0].files.length>0&&ze(t);return{c(){e=w("button"),e.textContent="Set time from client",n=C(),l=w("button"),l.textContent="Reboot ESP",s=C(),i=w("form"),r=w("label"),o=I(`Choose sequence file\r
      `),u=w("input"),c=C(),_&&_.c(),f(u,"type","file"),f(u,"name","file"),f(u,"accept",".seq"),f(u,"class","svelte-15k8ub1"),f(r,"class","button"),f(i,"class","file-form svelte-15k8ub1"),f(i,"action",a=`http://${t[1]}/upload`),f(i,"method","post"),f(i,"enctype","multipart/form-data")},m(h,$){k(h,e,$),k(h,n,$),k(h,l,$),k(h,s,$),k(h,i,$),b(i,r),b(r,o),b(r,u),t[5](u),b(i,c),_&&_.m(i,null),d||(m=[j(e,"click",t[2]),j(l,"click",t[3]),j(u,"change",t[4])],d=!0)},p(h,$){h[0]&&h[0].files.length>0?_?_.p(h,$):(_=ze(h),_.c(),_.m(i,null)):_&&(_.d(1),_=null),$&2&&a!==(a=`http://${h[1]}/upload`)&&f(i,"action",a)},d(h){h&&(v(e),v(n),v(l),v(s),v(i)),t[5](null),_&&_.d(),d=!1,X(m)}}}function tn(t){let e;return{c(){e=w("h2"),e.textContent="Server",f(e,"slot","heading")},m(n,l){k(n,e,l)},p:E,d(n){n&&v(e)}}}function nn(t){let e,n;return e=new ge({props:{name:"server",$$slots:{heading:[tn],default:[en]},$$scope:{ctx:t}}}),{c(){F(e.$$.fragment)},m(l,s){M(e,l,s),n=!0},p(l,[s]){const i={};s&131&&(i.$$scope={dirty:s,ctx:l}),e.$set(i)},i(l){n||(S(e.$$.fragment,l),n=!0)},o(l){P(e.$$.fragment,l),n=!1},d(l){A(e,l)}}}function ln(t,e,n){let l,s;L(t,_e,a=>n(1,l=a)),L(t,me,a=>n(6,s=a));let i;function r(){s.send(JSON.stringify({command:"setTime",time:new Date().getTime()}))}function o(){fetch(`http://${l}/reboot`,{method:"POST"})}function u(){n(0,i)}function c(a){ve[a?"unshift":"push"](()=>{i=a,n(0,i)})}return[i,l,r,o,u,c]}class sn extends V{constructor(e){super(),K(this,e,ln,nn,B,{})}}function He(t,e,n){const l=t.slice();return l[2]=e[n],l}function Je(t){let e,n=t[2].message+"",l,s,i;return{c(){e=w("p"),l=I(n),s=C(),f(e,"class",i=t[2].isError?"error":"success")},m(r,o){k(r,e,o),b(e,l),b(e,s)},p(r,o){o&2&&n!==(n=r[2].message+"")&&Q(l,n),o&2&&i!==(i=r[2].isError?"error":"success")&&f(e,"class",i)},d(r){r&&v(e)}}}function on(t){let e,n,l,s,i,r=le(t[1]),o=[];for(let u=0;u<r.length;u+=1)o[u]=Je(He(t,r,u));return{c(){e=w("div"),n=w("strong"),l=I(t[0]),s=I(":"),i=C();for(let u=0;u<o.length;u+=1)o[u].c()},m(u,c){k(u,e,c),b(e,n),b(n,l),b(n,s),b(e,i);for(let a=0;a<o.length;a+=1)o[a]&&o[a].m(e,null)},p(u,[c]){if(c&1&&Q(l,u[0]),c&2){r=le(u[1]);let a;for(a=0;a<r.length;a+=1){const d=He(u,r,a);o[a]?o[a].p(d,c):(o[a]=Je(d),o[a].c(),o[a].m(e,null))}for(;a<o.length;a+=1)o[a].d(1);o.length=r.length}},i:E,o:E,d(u){u&&v(e),Ce(o,u)}}}function rn(t,e,n){let{title:l}=e,{logs:s}=e;return t.$$set=i=>{"title"in i&&n(0,l=i.title),"logs"in i&&n(1,s=i.logs)},[l,s]}class un extends V{constructor(e){super(),K(this,e,rn,on,B,{title:0,logs:1})}}function Ke(t,e,n){const l=t.slice();return l[1]=e[n],l}function Ve(t){let e,n;return e=new un({props:{title:t[1].name,logs:t[1].logs}}),{c(){F(e.$$.fragment)},m(l,s){M(e,l,s),n=!0},p(l,s){const i={};s&1&&(i.title=l[1].name),s&1&&(i.logs=l[1].logs),e.$set(i)},i(l){n||(S(e.$$.fragment,l),n=!0)},o(l){P(e.$$.fragment,l),n=!1},d(l){A(e,l)}}}function cn(t){let e,n,l=le(t[0].states),s=[];for(let r=0;r<l.length;r+=1)s[r]=Ve(Ke(t,l,r));const i=r=>P(s[r],1,1,()=>{s[r]=null});return{c(){for(let r=0;r<s.length;r+=1)s[r].c();e=he()},m(r,o){for(let u=0;u<s.length;u+=1)s[u]&&s[u].m(r,o);k(r,e,o),n=!0},p(r,o){if(o&1){l=le(r[0].states);let u;for(u=0;u<l.length;u+=1){const c=Ke(r,l,u);s[u]?(s[u].p(c,o),S(s[u],1)):(s[u]=Ve(c),s[u].c(),S(s[u],1),s[u].m(e.parentNode,e))}for(qe(),u=l.length;u<s.length;u+=1)i(u);Pe()}},i(r){if(!n){for(let o=0;o<l.length;o+=1)S(s[o]);n=!0}},o(r){s=s.filter(Boolean);for(let o=0;o<s.length;o+=1)P(s[o]);n=!1},d(r){r&&v(e),Ce(s,r)}}}function fn(t){let e;return{c(){e=w("h2"),e.textContent="Status",f(e,"slot","heading")},m(n,l){k(n,e,l)},p:E,d(n){n&&v(e)}}}function an(t){let e,n;return e=new ge({props:{name:"status",$$slots:{heading:[fn],default:[cn]},$$scope:{ctx:t}}}),{c(){F(e.$$.fragment)},m(l,s){M(e,l,s),n=!0},p(l,[s]){const i={};s&17&&(i.$$scope={dirty:s,ctx:l}),e.$set(i)},i(l){n||(S(e.$$.fragment,l),n=!0)},o(l){P(e.$$.fragment,l),n=!1},d(l){A(e,l)}}}function dn(t,e,n){let l;return L(t,re,s=>n(0,l=s)),[l]}class pn extends V{constructor(e){super(),K(this,e,dn,an,B,{})}}function _n(t){let e,n,l;return{c(){e=w("button"),e.innerHTML='<img src="/icon.svg" alt="Crescent icon"/>',f(e,"title","Switch theme"),f(e,"class","svelte-1oalwmk")},m(s,i){k(s,e,i),n||(l=j(e,"click",t[0]),n=!0)},p:E,i:E,o:E,d(s){s&&v(e),n=!1,l()}}}function mn(t,e,n){let{theme:l=localStorage.getItem("theme")||(window.matchMedia("(prefers-color-scheme: dark)").matches?"dark":"light")}=e;function s(){const r=window.document.documentElement.classList;l=="dark"?r.add("dark"):r.remove("dark")}function i(){n(1,l=l=="light"?"dark":"light"),localStorage.setItem("theme",l),s()}return s(),t.$$set=r=>{"theme"in r&&n(1,l=r.theme)},[i,l]}class hn extends V{constructor(e){super(),K(this,e,mn,_n,B,{theme:1})}}function We(t){let e,n,l,s;return e=new sn({}),l=new xt({}),{c(){F(e.$$.fragment),n=C(),F(l.$$.fragment)},m(i,r){M(e,i,r),k(i,n,r),M(l,i,r),s=!0},i(i){s||(S(e.$$.fragment,i),S(l.$$.fragment,i),s=!0)},o(i){P(e.$$.fragment,i),P(l.$$.fragment,i),s=!1},d(i){i&&v(n),A(e,i),A(l,i)}}}function gn(t){let e,n,l,s,i,r,o,u,c,a,d;i=new hn({}),o=new pn({}),c=new Ht({});let m=t[0]&&We();return{c(){e=w("main"),n=w("div"),l=w("h1"),l.textContent="ESP Sequent Driver",s=C(),F(i.$$.fragment),r=C(),F(o.$$.fragment),u=C(),F(c.$$.fragment),a=C(),m&&m.c(),f(n,"class","svelte-p108mq"),f(e,"class","svelte-p108mq")},m(_,h){k(_,e,h),b(e,n),b(n,l),b(n,s),M(i,n,null),b(e,r),M(o,e,null),b(e,u),M(c,e,null),b(e,a),m&&m.m(e,null),d=!0},p(_,[h]){_[0]?m?h&1&&S(m,1):(m=We(),m.c(),S(m,1),m.m(e,null)):m&&(qe(),P(m,1,1,()=>{m=null}),Pe())},i(_){d||(S(i.$$.fragment,_),S(o.$$.fragment,_),S(c.$$.fragment,_),S(m),d=!0)},o(_){P(i.$$.fragment,_),P(o.$$.fragment,_),P(c.$$.fragment,_),P(m),d=!1},d(_){_&&v(e),A(i),A(o),A(c),m&&m.d()}}}function $n(t,e,n){let l;return L(t,ae,s=>n(0,l=s)),[l]}class bn extends V{constructor(e){super(),K(this,e,$n,gn,B,{})}}new bn({target:document.getElementById("app")});</script>
    <style>:root{font-family:Inter,system-ui,Avenir,Helvetica,Arial,sans-serif;line-height:1.5;font-weight:400;font-size:16px;box-sizing:border-box;color:var(--gray-5);background-color:var(--gray-95);transition:background-color .2s,color .2s}*,*:before,*:after{box-sizing:inherit}body,h1,h2,h3,h4,h5,h6,p,ol,ul{margin:0;padding:0;font-weight:400}ol,ul{list-style:none}img{max-width:100%;height:auto}a{font-weight:500;color:#646cff;text-decoration:inherit}a:hover{color:#535bf2}body{margin:0;display:flex}:root{--black: #121212;--white: #f0f0f0;--dark: #4c4c4c;--light: #acacac;--darker: #2d2d2d;--darkest: #1e1e1e;--lighter: #d1d1d1;--lightest: #e0e0e0;--medium: #888888;--light-red: #ff8484;--dark-red: #b70000;--light-green: #87ff85;--dark-green: #028100;--light-yellow: #fffd85;--dark-yellow: #768500}:root:not(.dark){--gray-5: var(--black);--gray-10: var(--darkest);--gray-15: var(--darker);--gray-35: var(--dark);--gray-50: var(--medium);--gray-65: var(--light);--gray-85: var(--lighter);--gray-90: var(--lightest);--gray-95: var(--white);--green-5: var(--dark-green);--green-95: var(--light-green);--red-5: var(--dark-red);--red-95: var(--light-red);--yellow-5: var(--dark-yellow);--yellow-95: var(--light-yellow)}:root.dark{--gray-5: var(--white);--gray-10: var(--lightest);--gray-15: var(--lighter);--gray-35: var(--light);--gray-50: var(--medium);--gray-65: var(--dark);--gray-85: var(--darker);--gray-90: var(--darkest);--gray-95: var(--black);--green-5: var(--light-green);--green-95: var(--dark-green);--red-5: var(--light-red);--red-95: var(--dark-red);--yellow-5: var(--light-yellow);--yellow-95: var(--dark-yellow)}#app{width:100%;max-width:480px;margin:0 auto;padding:2rem}button,.button{border-radius:8px;border:1px solid transparent;padding:.6em 1.2em;font-size:1em;font-weight:500;font-family:inherit;color:var(--gray-95);background-color:var(--gray-5);cursor:pointer;transition:background-color .2s,color .2s;display:block;text-align:center}button:hover,.button:hover{background-color:var(--gray-35)}button:disabled{background-color:var(--gray-50);cursor:not-allowed}label{display:flex;align-items:center;gap:16px;background-color:var(--gray-90);padding:6px 16px;border-radius:8px;transition:background-color .2s}label>span{flex-shrink:0}input{border-width:2px;border-color:transparent;border-style:solid;border-radius:8px;padding:.6em;outline:none;font-size:inherit;font-family:inherit;color:var(--gray-5);background-color:var(--gray-85);transition:background-color .2s,color .2s,border-color .2s;width:100%}input:hover{border-color:var(--gray-65)}input:focus{border-color:var(--gray-50)}input:disabled{border-color:transparent;color:var(--gray-50);cursor:not-allowed}.success{color:var(--green-5)}.warning{color:var(--yellow-5)}.error{color:var(--red-5)}p{white-space:pre-line;overflow-wrap:break-word}.section.svelte-sext2k{border-style:solid;border-width:2px;border-color:var(--gray-50);border-radius:16px;display:flex;flex-direction:column;padding:16px 16px 0;transition:border-color .2s}.body.svelte-sext2k{display:flex;flex-direction:column;row-gap:16px;padding-bottom:16px}.heading.svelte-sext2k{all:unset;display:flex;flex-direction:row;justify-content:space-between;align-items:center;padding-bottom:16px}.arrow-container.svelte-sext2k{transition:background-color .2s;background-color:transparent;border-radius:8px;padding:4px;display:flex;align-items:center}.arrow-container.svelte-sext2k:hover{background-color:var(--gray-65)}svg.svelte-sext2k{transition:transform .3s;color:inherit}svg.rotated.svelte-sext2k{transform:rotate(-90deg)}.sequence-container.svelte-104po3j{border-radius:8px;padding:1em;color:var(--gray-5);background-color:var(--gray-90);transition:background-color .2s,color .2s;width:100%}.sequence-container-inner.svelte-104po3j{display:flex;align-items:center;gap:8px}.status.svelte-104po3j{color:var(--gray-35)}input[type=file].svelte-15k8ub1{display:none}.file-form.svelte-15k8ub1{display:flex;flex-direction:column;row-gap:16px}button.svelte-1oalwmk{background-color:transparent;padding:8px;min-width:48px;display:flex;align-items:center}button.svelte-1oalwmk:hover{background-color:var(--gray-65)}div.svelte-p108mq{display:flex;flex-direction:row;justify-content:space-between;column-gap:8px}main.svelte-p108mq{display:flex;flex-direction:column;row-gap:32px}</style>
  </head>
  <body>
    <div id="app"></div>
  </body>
</html>
)rawliteral";

const char iconSvg[] PROGMEM = R"rawliteral(
<svg class="iconify iconify--logos" width="32" height="32" aria-hidden="true" preserveAspectRatio="xMidYMid" role="img" version="1.1" viewBox="0 0 256.96 257" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"><defs><radialGradient id="radialGradient9277" cx="128.48" cy="128.5" r="96.375" gradientUnits="userSpaceOnUse"><stop stop-color="#e6e6e6" offset="0"/><stop stop-color="#c6c6c6" offset=".67716"/><stop stop-color="#a5a5a5" offset="1"/></radialGradient><clipPath id="clipPath10832"><circle cx="128.48" cy="128.5" r="128.5" fill="none" style="paint-order:normal"/></clipPath></defs><g clip-path="url(#clipPath10832)"><rect x="-.01819" width="257" height="257" fill="#121212" style="paint-order:normal"/><circle cx="128.48" cy="128.5" r="96.375" fill="url(#radialGradient9277)" style="paint-order:normal"/><circle cx="96.357" cy="160.62" r="96.375" fill="#121212" style="paint-order:normal"/></g></svg>
)rawliteral";