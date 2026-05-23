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

    /* Devices tab */
    .dev-card{background:#1e293b;border:1px solid #334155;border-radius:12px;
              padding:14px 16px;margin-bottom:10px}
    .dev-card-header{display:flex;justify-content:space-between;align-items:center;margin-bottom:10px}
    .dev-name{font-size:.9rem;font-weight:600;color:#f1f5f9}
    .dev-id{font-size:.68rem;color:#475569;margin-top:2px}
    .dev-del{background:none;border:none;color:#64748b;font-size:1rem;cursor:pointer;padding:4px}
    .dev-del:active{color:#f87171}
    .cmd-chips{display:flex;flex-wrap:wrap;gap:6px}
    .cmd-chip{padding:7px 12px;border:none;border-radius:8px;background:#0f172a;
              color:#94a3b8;font-size:.75rem;font-weight:600;cursor:pointer;
              border:1px solid #1e293b;transition:background .12s}
    .cmd-chip:active{background:#334155;color:#e2e8f0}
    .add-dev-form{background:#1e293b;border:1px solid #334155;border-radius:12px;
                  padding:16px;margin-bottom:14px}
    .form-row{display:flex;gap:8px;margin-bottom:8px}
    .form-input{flex:1;padding:9px 12px;border-radius:8px;border:1px solid #334155;
                background:#0f172a;color:#e2e8f0;font-size:.82rem;outline:none}
    .form-input:focus{border-color:#38bdf8}
    .btn-sm{padding:9px 14px;border:none;border-radius:8px;background:#0369a1;
            color:#e0f2fe;font-size:.78rem;font-weight:600;cursor:pointer;white-space:nowrap}
    .btn-sm:active{background:#0284c7}
    .btn-sm.danger{background:#7f1d1d;color:#fca5a5}
    .section-title{font-size:.68rem;text-transform:uppercase;letter-spacing:.08em;
                   color:#475569;margin:18px 0 10px}
    .learn-picker{background:#1e293b;border:1px solid #38bdf8;border-radius:12px;
                  padding:14px;margin-bottom:14px;display:none}
    .learn-picker p{font-size:.78rem;color:#94a3b8;margin-bottom:10px}

    /* Learn tab */
    .learn-note{color:#64748b;font-size:.82rem;line-height:1.6;margin-bottom:20px;
                background:#1e293b;padding:14px;border-radius:10px;border:1px solid #334155}
    .learn-result{margin-top:16px;padding:16px;background:#1e293b;border-radius:12px;
                  border:1px solid #334155;display:none}
    .learn-result .kv{display:flex;justify-content:space-between;margin-bottom:8px;font-size:.82rem}
    .learn-result .kv .key{color:#64748b}
    .learn-result .kv .val{color:#38bdf8;font-family:monospace;font-weight:600}
    .learn-hint{margin-top:12px;font-size:.72rem;color:#475569;line-height:1.5}

    /* Settings modal */
    .modal-overlay{position:fixed;inset:0;background:rgba(0,0,0,.75);z-index:20;
                   display:none;align-items:flex-end;justify-content:center}
    .modal-overlay.open{display:flex}
    .modal-sheet{background:#1e293b;border-radius:20px 20px 0 0;padding:24px 20px 32px;
                 width:100%;max-width:480px}
    .modal-header{display:flex;justify-content:space-between;align-items:center;margin-bottom:18px}
    .modal-title{font-size:.95rem;font-weight:700;color:#f1f5f9}
    .modal-close{background:none;border:none;color:#64748b;font-size:1.3rem;cursor:pointer;
                 padding:4px 8px;line-height:1}
    .wifi-status-bar{font-size:.78rem;color:#94a3b8;background:#0f172a;padding:10px 12px;
                     border-radius:8px;margin-bottom:16px}
    .wifi-status-bar b{color:#4ade80}
    .form-label{font-size:.7rem;text-transform:uppercase;letter-spacing:.06em;
                color:#475569;margin-bottom:5px;display:block}
    .settings-btn{background:none;border:none;color:#475569;font-size:1.15rem;
                  cursor:pointer;padding:4px 6px;transition:color .15s}
    .settings-btn:active{color:#38bdf8}

    /* Save-to-device panel in learn tab */
    .save-panel{margin-top:14px;padding:14px 16px;background:#1e293b;
                border:1px solid #0369a1;border-radius:12px;display:none}
    .save-panel-title{font-size:.78rem;color:#38bdf8;font-weight:600;margin-bottom:10px}

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
  <div style="display:flex;align-items:center;gap:10px">
    <div id="net-info">IP: <span>...</span></div>
    <button class="settings-btn" onclick="openSettings()" title="C&#224;i &#273;&#7863;t WiFi">&#9881;&#65039;</button>
  </div>
</header>

<!-- ── Settings modal ──────────────────────────────────────────────────────── -->
<div class="modal-overlay" id="settings-modal">
  <div class="modal-sheet">
    <div class="modal-header">
      <span class="modal-title">&#9881;&#65039; C&#224;i &#273;&#7863;t WiFi</span>
      <button class="modal-close" onclick="closeSettings()">&#10005;</button>
    </div>
    <div class="wifi-status-bar">&#9679; &#272;ang d&#249;ng: <b id="ws-current-ssid">...</b></div>
    <label class="form-label">T&#234;n WiFi (SSID)</label>
    <div class="form-row" style="margin-bottom:12px">
      <input class="form-input" id="ws-ssid" placeholder="T&#234;n m&#7841;ng WiFi" autocomplete="off" />
    </div>
    <label class="form-label">M&#7853;t kh&#7849;u</label>
    <div class="form-row" style="margin-bottom:18px">
      <input class="form-input" id="ws-pass" type="password" placeholder="M&#7853;t kh&#7849;u WiFi" autocomplete="new-password" />
    </div>
    <button class="btn-sm" style="width:100%" onclick="saveWifi()">
      &#128190; L&#432;u &amp; kh&#7903;i &#273;&#7897;ng l&#7841;i ESP32
    </button>
    <p style="font-size:.7rem;color:#475569;text-align:center;margin-top:10px;line-height:1.5">
      ESP32 s&#7869; t&#7921; k&#7871;t n&#7889;i WiFi m&#7899;i sau khi kh&#7903;i &#273;&#7897;ng l&#7841;i.<br>
      M&#7903; tr&#236;nh duy&#7879;t l&#7841;i t&#7841;i IP m&#7899;i &#273;&#7875; ti&#7871;p t&#7909;c s&#7917; d&#7909;ng.
    </p>
  </div>
</div>

<div class="tabs">
  <button class="tab active" onclick="switchTab('tv')">&#128250; Tivi</button>
  <button class="tab" onclick="switchTab('ac')">&#10052;&#65039; &#272;H&#242;a</button>
  <button class="tab" onclick="switchTab('fan')">&#128168; Qu&#7841;t</button>
  <button class="tab" onclick="switchTab('devices')">&#128273; Thi&#7871;t b&#7883;</button>
  <button class="tab" onclick="switchTab('learn')">&#128225; H&#7885;c</button>
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

<!-- ── Devices ─────────────────────────────────────────────────────────────── -->
<div class="pane" id="pane-devices">
  <!-- Add device form -->
  <div class="section-title">Th&#234;m thi&#7871;t b&#7883;</div>
  <div class="add-dev-form">
    <div class="form-row">
      <input class="form-input" id="dev-id"   placeholder="ID (v&#237;d&#7909;: dieu_hoa_phong_ngu)" />
    </div>
    <div class="form-row">
      <input class="form-input" id="dev-name" placeholder="T&#234;n hi&#7875;n th&#7883; (v&#237;d&#7909;: &#272;i&#7873;u h&#242;a ph&#242;ng ng&#7911;)" />
      <button class="btn-sm" onclick="createDevice()">+ Th&#234;m</button>
    </div>
  </div>

  <!-- Learn & bind picker (shown when adding a command) -->
  <div class="learn-picker" id="learn-picker">
    <p>&#128225; H&#432;&#7899;ng &#273;i&#7873;u khi&#7875;n g&#7889;c v&#224;o m&#7855;t th&#7847;n v&#224; nh&#7845;n n&#250;t c&#7847;n h&#7885;c...</p>
    <div class="form-row" style="margin-bottom:6px">
      <input class="form-input" id="lp-label" placeholder="Nh&#227;n (v&#237; d&#7909;: B&#7853;t / T&#7855;t)" oninput="autoFillId('lp-label','lp-cmdname')" />
    </div>
    <div class="form-row">
      <input class="form-input" id="lp-cmdname" placeholder="ID l&#7879;nh (t&#7921; &#273;i&#7873;n)" oninput="this.dataset.manual='1'" />
      <button class="btn-sm" id="lp-btn" onclick="learnAndBind()">&#128225; H&#7885;c</button>
    </div>
    <button class="btn-sm danger" style="width:100%;margin-top:4px" onclick="closePicker()">H&#7911;y</button>
  </div>

  <!-- Device list -->
  <div class="section-title">Danh s&#225;ch thi&#7871;t b&#7883;</div>
  <div id="dev-list"><p style="color:#475569;font-size:.8rem">Ch&#432;a c&#243; thi&#7871;t b&#7883; n&#224;o.</p></div>
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
      G&#7885;i tr&#7921;c ti&#7871;p qua API: <code style="color:#38bdf8">POST /api/ir/raw</code>
    </p>
  </div>

  <div class="save-panel" id="save-panel">
    <div class="save-panel-title">&#128190; L&#432;u v&#224;o thi&#7871;t b&#7883;</div>
    <div class="form-row" style="margin-bottom:8px">
      <select class="form-input" id="save-dev-select" style="cursor:pointer"></select>
    </div>
    <div class="form-row" style="margin-bottom:6px">
      <input class="form-input" id="save-label" placeholder="Nh&#227;n (v&#237; d&#7909;: B&#7853;t / T&#7855;t)" oninput="autoFillId('save-label','save-cmd-name')" />
    </div>
    <div class="form-row">
      <input class="form-input" id="save-cmd-name" placeholder="ID l&#7879;nh (t&#7921; &#273;i&#7873;n)" oninput="this.dataset.manual='1'" />
      <button class="btn-sm" onclick="saveLearnedToDevice()">L&#432;u</button>
    </div>
  </div>
</div>

<div id="status-bar">&#272;ang k&#7871;t n&#7889;i...</div>

<script>
  // ── State ───────────────────────────────────────────────────────────────────
  let ac = { power: false, temp: 25, mode: 'cool', fan: 'auto' };

  // ── Helpers ─────────────────────────────────────────────────────────────────
  function toId(str) {
    return str.normalize('NFD')
      .replace(/[̀-ͯ]/g, '')
      .replace(/đ/g, 'd').replace(/Đ/g, 'd')
      .toLowerCase()
      .replace(/[^a-z0-9]+/g, '_')
      .replace(/^_+|_+$/g, '');
  }
  function autoFillId(srcId, dstId) {
    const dst = document.getElementById(dstId);
    if (!dst.dataset.manual) dst.value = toId(document.getElementById(srcId).value);
  }

  // ── Tab ─────────────────────────────────────────────────────────────────────
  const TAB_NAMES = ['tv','ac','fan','devices','learn'];
  function switchTab(name) {
    document.querySelectorAll('.tab').forEach((t, i) =>
      t.classList.toggle('active', TAB_NAMES[i] === name));
    document.querySelectorAll('.pane').forEach(p =>
      p.classList.toggle('active', p.id === 'pane-' + name));
    if (name === 'devices') loadDevices();
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

  async function showLearnResult(d) {
    document.getElementById('lr-proto').textContent = d.protocol;
    document.getElementById('lr-code').textContent  = d.code;
    document.getElementById('lr-bits').textContent  = d.bits + ' bit';
    document.getElementById('learn-result').style.display = 'block';
    setStatus('H&#7885;c m&#227; th&#224;nh c&#244;ng: ' + d.protocol + ' ' + d.code, 'ok');

    // populate save-to-device panel
    const sel = document.getElementById('save-dev-select');
    sel.innerHTML = '';
    try {
      const devs = await fetch('/api/devices').then(r => r.json());
      if (!devs.length) {
        sel.innerHTML = '<option value="">-- Ch&#432;a c&#243; thi&#7871;t b&#7883; (t&#7841;o &#7903; tab Thi&#7871;t b&#7883;) --</option>';
      } else {
        sel.innerHTML = '<option value="">-- Ch&#7885;n thi&#7871;t b&#7883; --</option>';
        devs.forEach(dv => {
          const o = document.createElement('option');
          o.value = dv.id;
          o.textContent = dv.name + ' (' + dv.id + ')';
          sel.appendChild(o);
        });
      }
    } catch(_) {
      sel.innerHTML = '<option value="">L&#7895;i t&#7843;i danh s&#225;ch</option>';
    }
    document.getElementById('save-label').value = '';
    const sc = document.getElementById('save-cmd-name');
    sc.value = ''; delete sc.dataset.manual;
    document.getElementById('save-panel').style.display = 'block';
  }

  async function saveLearnedToDevice() {
    const devId   = document.getElementById('save-dev-select').value;
    const label   = document.getElementById('save-label').value.trim();
    const cmdName = document.getElementById('save-cmd-name').value.trim();
    const proto   = document.getElementById('lr-proto').textContent;
    const code    = document.getElementById('lr-code').textContent;
    const bits    = parseInt(document.getElementById('lr-bits').textContent);
    if (!devId)   { setStatus('Ch&#7885;n thi&#7871;t b&#7883;', 'err'); return; }
    if (!cmdName) { setStatus('Nh&#7853;p ID l&#7879;nh', 'err'); return; }
    const r = await fetch('/api/devices/' + devId + '/cmds', {
      method: 'POST',
      headers: {'Content-Type':'application/json'},
      body: JSON.stringify({ cmd: cmdName, label, proto, code, bits })
    });
    if (r.ok) {
      setStatus('&#10003; &#272;&#227; l&#432;u l&#7879;nh "' + (label || cmdName) + '" v&#224;o thi&#7871;t b&#7883;', 'ok');
      document.getElementById('save-panel').style.display = 'none';
    } else {
      setStatus('L&#7895;i l&#432;u', 'err');
    }
  }

  // ── Devices ─────────────────────────────────────────────────────────────────
  let _pickerDevId = null;

  async function loadDevices() {
    try {
      const r = await fetch('/api/devices');
      const devs = await r.json();
      const list = document.getElementById('dev-list');
      if (!devs.length) {
        list.innerHTML = '<p style="color:#475569;font-size:.8rem">Ch&#432;a c&#243; thi&#7871;t b&#7883; n&#224;o.</p>';
        return;
      }
      list.innerHTML = devs.map(dev => {
        const cmdBtns = Object.entries(dev.cmds || {}).map(([key, val]) =>
          `<button class="cmd-chip" onclick="sendDevCmd('${dev.id}','${key}')">${val.label || key}</button>`
        ).join('');
        return `<div class="dev-card">
          <div class="dev-card-header">
            <div><div class="dev-name">${dev.name}</div><div class="dev-id">${dev.id}</div></div>
            <button class="dev-del" onclick="deleteDevice('${dev.id}')">&#128465;</button>
          </div>
          <div class="cmd-chips">${cmdBtns}
            <button class="cmd-chip" style="border-color:#0369a1;color:#38bdf8"
              onclick="openPicker('${dev.id}')">+ H&#7885;c l&#7879;nh</button>
          </div>
        </div>`;
      }).join('');
    } catch(e) { setStatus('L&#7895;i t&#7843;i danh s&#225;ch', 'err'); }
  }

  async function createDevice() {
    const id   = document.getElementById('dev-id').value.trim();
    const name = document.getElementById('dev-name').value.trim();
    if (!id || !name) { setStatus('Nh&#7853;p ID v&#224; t&#234;n thi&#7871;t b&#7883;', 'err'); return; }
    const r = await fetch('/api/devices', {
      method: 'POST',
      headers: {'Content-Type':'application/json'},
      body: JSON.stringify({id, name})
    });
    const d = await r.json();
    if (r.ok) {
      document.getElementById('dev-id').value = '';
      document.getElementById('dev-name').value = '';
      setStatus('&#10003; &#272;&#227; th&#234;m thi&#7871;t b&#7883;: ' + name, 'ok');
      loadDevices();
    } else { setStatus('L&#7895;i: ' + d.error, 'err'); }
  }

  async function deleteDevice(id) {
    if (!confirm('X&#243;a thi&#7871;t b&#7883; ' + id + '?')) return;
    const r = await fetch('/api/devices/' + id, { method: 'DELETE' });
    setStatus(r.ok ? '&#10003; &#272;&#227; x&#243;a' : 'L&#7895;i x&#243;a', r.ok ? 'ok' : 'err');
    loadDevices();
  }

  async function sendDevCmd(id, cmd) {
    try {
      const r = await fetch('/api/devices/' + id + '/cmd/' + cmd, { method: 'POST' });
      setStatus(r.ok ? '&#10003; ' + id + ' &#8594; ' + cmd : 'L&#7895;i HTTP ' + r.status, r.ok ? 'ok' : 'err');
    } catch(e) { setStatus('L&#7895;i k&#7871;t n&#7889;i', 'err'); }
  }

  function openPicker(devId) {
    _pickerDevId = devId;
    document.getElementById('lp-label').value = '';
    const cmd = document.getElementById('lp-cmdname');
    cmd.value = ''; delete cmd.dataset.manual;
    document.getElementById('learn-picker').style.display = 'block';
    document.getElementById('lp-btn').disabled = false;
    document.getElementById('lp-btn').innerHTML = '&#128225; H&#7885;c';
  }

  function closePicker() {
    document.getElementById('learn-picker').style.display = 'none';
    _pickerDevId = null;
  }

  async function learnAndBind() {
    const label   = document.getElementById('lp-label').value.trim();
    const cmdName = document.getElementById('lp-cmdname').value.trim();
    if (!cmdName) { setStatus('Nh&#7853;p nh&#227;n l&#7879;nh', 'err'); return; }
    const btn = document.getElementById('lp-btn');
    btn.disabled = true;
    btn.innerHTML = '&#9203; &#272;ang ch&#7901;...';
    setStatus('H&#432;&#7899;ng &#273;i&#7873;u khi&#7875;n g&#7889;c v&#224;o m&#7855;t th&#7847;n v&#224; nh&#7845;n n&#250;t...');

    await fetch('/api/ir/learn', { method: 'POST' });

    let elapsed = 0;
    const t = setInterval(async () => {
      elapsed += 1000;
      const r  = await fetch('/api/ir/learn');
      const d  = await r.json();
      if (d.done) {
        clearInterval(t);
        // save to device
        await fetch('/api/devices/' + _pickerDevId + '/cmds', {
          method: 'POST',
          headers: {'Content-Type':'application/json'},
          body: JSON.stringify({ cmd: cmdName, label, proto: d.protocol, code: d.code, bits: d.bits })
        });
        setStatus('&#10003; L&#432;u l&#7879;nh "' + (label || cmdName) + '": ' + d.protocol + ' ' + d.code, 'ok');
        closePicker();
        loadDevices();
      } else if (elapsed >= 11000) {
        clearInterval(t);
        setStatus('Kh&#244;ng nh&#7853;n &#273;&#432;&#7907;c t&#237;n hi&#7879;u', 'err');
        btn.disabled = false;
        btn.innerHTML = '&#128225; H&#7885;c';
      }
    }, 1000);
  }

  // ── WiFi settings ───────────────────────────────────────────────────────────
  async function openSettings() {
    document.getElementById('settings-modal').classList.add('open');
    try {
      const d = await fetch('/api/wifi').then(r => r.json());
      document.getElementById('ws-current-ssid').textContent = d.ssid || '?';
      document.getElementById('ws-ssid').value = d.ssid || '';
    } catch(_) {}
  }

  function closeSettings() {
    document.getElementById('settings-modal').classList.remove('open');
  }

  async function saveWifi() {
    const ssid = document.getElementById('ws-ssid').value.trim();
    const pass = document.getElementById('ws-pass').value;
    if (!ssid) { setStatus('Nh&#7853;p t&#234;n WiFi', 'err'); return; }
    const r = await fetch('/api/wifi', {
      method: 'POST',
      headers: {'Content-Type':'application/json'},
      body: JSON.stringify({ ssid, password: pass })
    });
    if (r.ok) {
      closeSettings();
      setStatus('&#10003; &#272;&#227; l&#432;u — ESP32 &#273;ang kh&#7903;i &#273;&#7897;ng l&#7841;i...', 'ok');
    } else {
      const d = await r.json();
      setStatus('L&#7895;i: ' + (d.error || r.status), 'err');
    }
  }

  // close modal when clicking backdrop
  document.getElementById('settings-modal').addEventListener('click', function(e) {
    if (e.target === this) closeSettings();
  });

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
