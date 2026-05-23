#pragma once

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no">
  <title>IR Remote</title>
  <style>
    *,*::before,*::after{margin:0;padding:0;box-sizing:border-box}
    body{font-family:'Segoe UI',system-ui,sans-serif;background:#0f172a;color:#e2e8f0;
         max-width:480px;margin:0 auto;min-height:100vh;display:flex;flex-direction:column}

    /* Header */
    header{background:#1e293b;padding:14px 20px;display:flex;align-items:center;
           justify-content:space-between;box-shadow:0 2px 8px rgba(0,0,0,.4);flex-shrink:0}
    header h1{font-size:1rem;color:#f8fafc;letter-spacing:-.01em}
    #net-info{font-size:.7rem;color:#64748b;text-align:right}
    #net-info span{color:#38bdf8}

    /* Tabs */
    .tabs{display:flex;background:#1e293b;border-bottom:1px solid #1e293b;flex-shrink:0}
    .tab{flex:1;padding:13px 4px;border:none;background:none;color:#64748b;font-size:.75rem;
         font-weight:600;cursor:pointer;border-bottom:2px solid transparent;transition:all .2s;
         letter-spacing:.01em}
    .tab.active{color:#38bdf8;border-bottom-color:#38bdf8;background:rgba(56,189,248,.05)}

    /* Panes */
    .pane{display:none;padding:20px;flex:1;padding-bottom:72px}
    .pane.active{display:block}

    /* Generic buttons */
    .ir-btn{width:100%;padding:15px 12px;border:none;border-radius:12px;background:#1e293b;
            color:#cbd5e1;font-size:.88rem;font-weight:600;cursor:pointer;
            border:1px solid #334155;transition:background .12s,transform .08s;
            -webkit-tap-highlight-color:transparent;user-select:none}
    .ir-btn:active{transform:scale(.95);background:#334155}
    .ir-btn.pwr{background:#450a0a;color:#fca5a5;border-color:#7f1d1d}
    .ir-btn.pwr:active{background:#7f1d1d}
    .ir-btn.pwr.on{background:#052e16;color:#86efac;border-color:#166534}
    .ir-btn.pwr.on:active{background:#166534}

    /* TV grid */
    .tv-grid{display:grid;grid-template-columns:1fr 1fr;gap:10px}
    .full{grid-column:1/-1}

    /* AC */
    .ac-power-row{margin-bottom:20px}
    .temp-display{display:flex;align-items:center;justify-content:center;gap:24px;
                  padding:20px 0;margin-bottom:4px}
    .temp-val{font-size:4rem;font-weight:700;color:#38bdf8;line-height:1;
              min-width:3ch;text-align:center}
    .temp-unit{font-size:1.1rem;color:#64748b;align-self:flex-start;margin-top:8px}
    .temp-btn{width:56px;height:56px;border:none;border-radius:50%;background:#1e293b;
              color:#e2e8f0;font-size:1.6rem;font-weight:700;cursor:pointer;
              border:1px solid #334155;transition:background .12s,transform .08s;
              -webkit-tap-highlight-color:transparent}
    .temp-btn:active{transform:scale(.92);background:#334155}
    .group-label{font-size:.68rem;text-transform:uppercase;letter-spacing:.08em;
                 color:#475569;margin:18px 0 8px}
    .chip-row{display:flex;gap:8px;flex-wrap:wrap}
    .chip{flex:1;min-width:56px;padding:10px 8px;border:none;border-radius:10px;
          background:#1e293b;color:#64748b;font-size:.78rem;font-weight:600;
          cursor:pointer;border:1px solid #334155;transition:all .15s;
          text-align:center;-webkit-tap-highlight-color:transparent}
    .chip:active{transform:scale(.95)}
    .chip.active{background:#0369a1;color:#e0f2fe;border-color:#0284c7}

    /* Fan grid */
    .fan-grid{display:grid;grid-template-columns:1fr 1fr 1fr;gap:10px}

    /* Learn tab */
    .learn-note{color:#64748b;font-size:.82rem;line-height:1.6;margin-bottom:20px;
                background:#1e293b;padding:14px;border-radius:10px;border:1px solid #334155}
    .learn-result{margin-top:16px;padding:16px;background:#1e293b;border-radius:12px;
                  border:1px solid #334155;display:none}
    .learn-result .kv{display:flex;justify-content:space-between;margin-bottom:8px;font-size:.82rem}
    .learn-result .kv .key{color:#64748b}
    .learn-result .kv .val{color:#38bdf8;font-family:monospace;font-weight:600}
    .learn-hint{margin-top:12px;font-size:.72rem;color:#475569;line-height:1.5}

    /* Status bar */
    #status-bar{position:fixed;bottom:0;left:50%;transform:translateX(-50%);width:100%;
                max-width:480px;padding:10px 20px;font-size:.72rem;color:#64748b;
                text-align:center;background:#0f172a;border-top:1px solid #1e293b;z-index:10}
    #status-bar.err{color:#f87171}
    #status-bar.ok{color:#4ade80}
  </style>
</head>
<body>
<header>
  <h1>&#128308; IR Remote</h1>
  <div id="net-info">IP: <span>...</span></div>
</header>

<div class="tabs">
  <button class="tab active" onclick="switchTab('tv')">&#128250; Tivi</button>
  <button class="tab" onclick="switchTab('ac')">&#10052;&#65039; &#272;i&#7873;u h&#242;a</button>
  <button class="tab" onclick="switchTab('fan')">&#128168; Qu&#7841;t</button>
  <button class="tab" onclick="switchTab('learn')">&#128225; H&#7885;c m&#227;</button>
</div>

<!-- ── TV ─────────────────────────────────────────────────────────────────── -->
<div class="pane active" id="pane-tv">
  <div class="tv-grid">
    <button class="ir-btn pwr full" onclick="irSend('tv','power')">&#9211; B&#7853;t / T&#7855;t</button>
    <button class="ir-btn" onclick="irSend('tv','vol_up')">&#128266; VOL +</button>
    <button class="ir-btn" onclick="irSend('tv','ch_up')">&#9650; K&#234;nh +</button>
    <button class="ir-btn" onclick="irSend('tv','mute')">&#128263; T&#7855;t ti&#7871;ng</button>
    <button class="ir-btn" onclick="irSend('tv','ch_dn')">&#9660; K&#234;nh &minus;</button>
    <button class="ir-btn" onclick="irSend('tv','vol_dn')">&#128265; VOL &minus;</button>
    <button class="ir-btn full" onclick="irSend('tv','source')">&#128250; Ngu&#7891;n v&#224;o</button>
  </div>
</div>

<!-- ── AC ─────────────────────────────────────────────────────────────────── -->
<div class="pane" id="pane-ac">
  <div class="ac-power-row">
    <button class="ir-btn pwr" id="ac-pwr" onclick="acTogglePower()">&#9211; B&#7853;t &#273;i&#7873;u h&#242;a</button>
  </div>

  <div class="temp-display">
    <button class="temp-btn" onclick="acAdjTemp(-1)">&#8722;</button>
    <div>
      <span class="temp-val" id="ac-temp">25</span>
      <span class="temp-unit">&#176;C</span>
    </div>
    <button class="temp-btn" onclick="acAdjTemp(+1)">&#43;</button>
  </div>

  <div class="group-label">Ch&#7871; &#273;&#7897;</div>
  <div class="chip-row">
    <button class="chip active" id="mode-cool" onclick="acSetMode('cool')">&#10052; L&#7841;nh</button>
    <button class="chip" id="mode-heat" onclick="acSetMode('heat')">&#128293; N&#243;ng</button>
    <button class="chip" id="mode-fan"  onclick="acSetMode('fan')">&#128168; Qu&#7841;t</button>
    <button class="chip" id="mode-dry"  onclick="acSetMode('dry')">&#128167; Kh&#244;</button>
  </div>

  <div class="group-label">T&#7889;c &#273;&#7897; qu&#7841;t</div>
  <div class="chip-row">
    <button class="chip active" id="fan-auto" onclick="acSetFan('auto')">T&#7921; &#273;&#7897;ng</button>
    <button class="chip" id="fan-low"  onclick="acSetFan('low')">Th&#7845;p</button>
    <button class="chip" id="fan-med"  onclick="acSetFan('med')">V&#7915;a</button>
    <button class="chip" id="fan-high" onclick="acSetFan('high')">Cao</button>
  </div>
</div>

<!-- ── Fan ────────────────────────────────────────────────────────────────── -->
<div class="pane" id="pane-fan">
  <div class="fan-grid">
    <button class="ir-btn pwr" style="grid-column:1/-1" onclick="irSend('fan','power')">&#9211; B&#7853;t / T&#7855;t</button>
    <button class="ir-btn" onclick="irSend('fan','spd1')">&#128168; T&#7889;c 1</button>
    <button class="ir-btn" onclick="irSend('fan','spd2')">&#128168;&#128168; T&#7889;c 2</button>
    <button class="ir-btn" onclick="irSend('fan','spd3')">&#128168;&#128168;&#128168; T&#7889;c 3</button>
    <button class="ir-btn" style="grid-column:1/-1" onclick="irSend('fan','swing')">&#8596;&#65039; Xoay</button>
  </div>
</div>

<!-- ── Learn ──────────────────────────────────────────────────────────────── -->
<div class="pane" id="pane-learn">
  <div class="learn-note">
    &#128161; H&#432;&#7899;ng m&#7855;t th&#7847;n nh&#7853;n (GPIO <b>IR_RX_PIN</b>) ra xa, sau &#273;&#243; h&#432;&#7899;ng &#273;i&#7873;u khi&#7875;n g&#7889;c v&#224;o m&#7855;t th&#7847;n v&#224; nh&#7845;n n&#250;t c&#7847;n h&#7885;c. ESP32 s&#7869; gi&#7843;i m&#227; giao th&#7913;c v&#224; m&#227; hex &#273;&#7875; b&#7841;n &#273;&#237;n v&#224;o config.h.
  </div>
  <button class="ir-btn" id="learn-btn" onclick="startLearn()">&#128225; B&#7855;t &#273;&#7847;u h&#7885;c m&#227; (10s)</button>

  <div class="learn-result" id="learn-result">
    <div class="kv"><span class="key">Giao th&#7913;c</span><span class="val" id="lr-proto">—</span></div>
    <div class="kv"><span class="key">M&#227; hex</span><span class="val" id="lr-code">—</span></div>
    <div class="kv"><span class="key">S&#7889; bit</span><span class="val" id="lr-bits">—</span></div>
    <p class="learn-hint">
      Sao ch&#233;p v&#224;o config.h:<br>
      <code style="color:#38bdf8">#define MY_CMD &lt;m&#227; hex&gt;UL</code><br>
      R&#7891;i s&#7917; d&#7909;ng <code style="color:#38bdf8">irSendNEC(MY_CMD)</code> ho&#7863;c g&#7885;i qua <code style="color:#38bdf8">POST /api/ir/raw</code>.
    </p>
  </div>
</div>

<div id="status-bar">&#272;ang k&#7871;t n&#7889;i...</div>

<script>
  // ── State ───────────────────────────────────────────────────────────────────
  let ac = { power: false, temp: 25, mode: 'cool', fan: 'auto' };

  // ── Tab ─────────────────────────────────────────────────────────────────────
  const TAB_NAMES = ['tv','ac','fan','learn'];
  function switchTab(name) {
    document.querySelectorAll('.tab').forEach((t, i) =>
      t.classList.toggle('active', TAB_NAMES[i] === name));
    document.querySelectorAll('.pane').forEach(p =>
      p.classList.toggle('active', p.id === 'pane-' + name));
  }

  // ── Status bar ──────────────────────────────────────────────────────────────
  function setStatus(msg, type) {
    const el = document.getElementById('status-bar');
    el.innerHTML = msg;
    el.className = type || '';
  }

  // ── IR send — TV / Fan ──────────────────────────────────────────────────────
  async function irSend(device, cmd) {
    try {
      const r = await fetch('/api/ir', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ device, cmd })
      });
      setStatus(r.ok ? '&#10003; ' + device + ' → ' + cmd : 'L&#7895;i HTTP ' + r.status, r.ok ? 'ok' : 'err');
    } catch (e) { setStatus('L&#7895;i k&#7871;t n&#7889;i', 'err'); }
  }

  // ── AC ──────────────────────────────────────────────────────────────────────
  function renderAc() {
    document.getElementById('ac-temp').textContent = ac.temp;
    const btn = document.getElementById('ac-pwr');
    btn.className = 'ir-btn pwr' + (ac.power ? ' on' : '');
    btn.innerHTML = ac.power
      ? '&#9211; &#272;ang b&#7853;t &mdash; Nh&#7845;n &#273;&#7875; t&#7855;t'
      : '&#9211; B&#7853;t &#273;i&#7873;u h&#242;a';
    ['cool','heat','fan','dry'].forEach(m =>
      document.getElementById('mode-' + m).classList.toggle('active', ac.mode === m));
    ['auto','low','med','high'].forEach(f =>
      document.getElementById('fan-' + f).classList.toggle('active', ac.fan === f));
  }

  async function sendAc(patch) {
    Object.assign(ac, patch);
    renderAc();
    try {
      const r = await fetch('/api/ir/ac', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(ac)
      });
      if (r.ok) {
        setStatus((ac.power ? 'B&#7853;t' : 'T&#7855;t') + ' &mdash; ' + ac.temp + '&#176;C &mdash; ' + ac.mode + ' &mdash; qu&#7841;t ' + ac.fan, 'ok');
      } else {
        setStatus('L&#7895;i &#273;i&#7873;u h&#242;a: HTTP ' + r.status, 'err');
      }
    } catch (e) { setStatus('L&#7895;i k&#7871;t n&#7889;i', 'err'); }
  }

  function acTogglePower() { sendAc({ power: !ac.power }); }
  function acAdjTemp(d)    { sendAc({ temp: Math.max(16, Math.min(30, ac.temp + d)) }); }
  function acSetMode(m)    { sendAc({ mode: m }); }
  function acSetFan(f)     { sendAc({ fan: f }); }

  // ── Learn ───────────────────────────────────────────────────────────────────
  let _pollTimer = null;

  async function startLearn() {
    const btn = document.getElementById('learn-btn');
    btn.disabled = true;
    btn.innerHTML = '&#9203; &#272;ang ch&#7901;... (10s)';
    document.getElementById('learn-result').style.display = 'none';
    setStatus('H&#432;&#7899;ng &#273;i&#7873;u khi&#7875;n g&#7889;c v&#224;o m&#7855;t th&#7847;n v&#224; nh&#7845;n n&#250;t...');

    try {
      await fetch('/api/ir/learn', { method: 'POST' });
    } catch (e) {
      setStatus('L&#7895;i: ' + e.message, 'err');
      resetLearnBtn(btn); return;
    }

    let elapsed = 0;
    _pollTimer = setInterval(async () => {
      elapsed += 1000;
      try {
        const r = await fetch('/api/ir/learn');
        const d = await r.json();
        if (d.done) {
          clearInterval(_pollTimer);
          showLearnResult(d);
          resetLearnBtn(btn);
        } else if (elapsed >= 11000) {
          clearInterval(_pollTimer);
          setStatus('Kh&#244;ng nh&#7853;n &#273;&#432;&#7907;c t&#237;n hi&#7879;u. Th&#7917; l&#7841;i.', 'err');
          resetLearnBtn(btn);
        }
      } catch (_) {}
    }, 1000);
  }

  function resetLearnBtn(btn) {
    btn.disabled = false;
    btn.innerHTML = '&#128225; B&#7855;t &#273;&#7847;u h&#7885;c m&#227; (10s)';
  }

  function showLearnResult(d) {
    document.getElementById('lr-proto').textContent = d.protocol;
    document.getElementById('lr-code').textContent  = d.code;
    document.getElementById('lr-bits').textContent  = d.bits + ' bit';
    document.getElementById('learn-result').style.display = 'block';
    setStatus('H&#7885;c m&#227; th&#224;nh c&#244;ng: ' + d.protocol + ' ' + d.code, 'ok');
  }

  // ── Init ────────────────────────────────────────────────────────────────────
  async function loadStatus() {
    try {
      const r = await fetch('/api/status');
      const s = await r.json();
      document.getElementById('net-info').innerHTML =
        'IP: <span>' + s.ip + '</span> &nbsp; ' + s.ssid + ' &nbsp; ' + s.rssi + 'dBm';
    } catch (_) {}
  }

  renderAc();
  loadStatus();
  setInterval(loadStatus, 30000);
</script>
</body>
</html>
)rawliteral";
