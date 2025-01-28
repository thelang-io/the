#!/usr/bin/env node

/*!
 * Copyright (c) 2018 Aaron Delasy
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const EventEmitter = require('events')
const childProcess = require('child_process')
const crypto = require('crypto')
const fs = require('fs')
const fsPromises = require('fs/promises')
const http = require('http')
const path = require('path')

const appBinaryPath = 'build/the'
const appDepsDir = process.env.THE_DEPS_DIR
const buildTime = Date.now()

async function exec (command, options) {
  return await new Promise((resolve) => {
    childProcess.exec(command, options, (err, stdout, stderr) => {
      resolve({
        code: err?.code ?? 0,
        stderr,
        stdout
      })
    })
  })
}

async function tmpFile (body) {
  const buf = await crypto.randomBytes(3)
  const filePath = path.resolve(process.cwd(), `tmp-${buf.readUIntBE(0, 3)}`)
  const outputPath = `${filePath}.out`
  await fsPromises.writeFile(filePath, body, 'utf8')
  return { filePath, outputPath }
}

const doctypeHTML = '<!doctype html>'
const homeHTML = doctypeHTML + `<html lang="en">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
    <link rel="icon" href="https://docs.thelang.io/favicon.ico" type="image/x-icon" />
    <title>The Playground</title>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css" integrity="sha512-iecdLmaskl7CVkqkXNQ/ZH/XLlvWZOJyj7Yy7tcenmpD1ypASozpmT/E0iPtmFIB46ZmdtAc9eNBvH0H/ZpiBw==" crossorigin="anonymous" referrerpolicy="no-referrer" />
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.39.0/min/vs/editor/editor.main.min.css" integrity="sha512-/hTKJ6YcK7JBJmV7HzjxM7LDShlTlamVMPdB0CaQRV5NeVS+ZVx8MogcT8Rw0DMRVGT7rNE+mDSc2QEVuJNdNA==" crossorigin="anonymous" referrerpolicy="no-referrer" />
    <link rel="stylesheet" href="/style.css?t=${buildTime}" />
  </head>
  <body>
    <div class="app">
      <div class="sidebar" id="sidebar">
        <button class="sidebar__action" id="build" type="button">
          <i class="fa-solid fa-wrench"></i>
        </button>
        <button class="sidebar__action sidebar__action--blue" id="run" type="button">
          <i class="fa-solid fa-play"></i>
        </button>
      </div>
      <div class="editors" id="editors" style="opacity: 0;">
        <div class="editor__wrapper">
          <div class="editor" id="editor-the"></div>
          <div class="separator horizontal" id="separator2" style="display: none;"></div>
          <div class="output" id="output" style="display: none;">
            <div class="output__header">
              <div class="output__header-headline">Output</div>
              <div class="output__header-actions">
                <button class="output__header-action" id="output-close">
                  <i class="fa-solid fa-xmark"></i>
                </button>
              </div>
            </div>
            <div class="output__content-wrapper">
              <pre class="output__content" id="output-content"></pre>
            </div>
          </div>
        </div>
        <div class="separator vertical" id="separator1"></div>
        <div class="editor__wrapper">
          <div class="editor" id="editor-c"></div>
        </div>
      </div>
    </div>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/lodash.js/4.17.21/lodash.min.js" integrity="sha512-WFN04846sdKMIP5LKNphMaWzU7YpMyCU245etK3g/2ARYbPK9Ub18eG+ljU96qKRCWh+quCY7yefSmlkQw1ANQ==" crossorigin="anonymous" referrerpolicy="no-referrer"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.39.0/min/vs/loader.min.js" integrity="sha512-A+6SvPGkIN9Rf0mUXmW4xh7rDvALXf/f0VtOUiHlDUSPknu2kcfz1KzLpOJyL2pO+nZS13hhIjLqVgiQExLJrw==" crossorigin="anonymous" referrerpolicy="no-referrer"></script>
    <script src="/common.js?t=${buildTime}"></script>
  </body>
</html>`

const styleCSS = `body, html {
  height: 100%;
  width: 100%;
}
body {
  margin: 0;
  padding: 0;
}
*, *::after, *::before {
  box-sizing: border-box;
}
body.resizing-vertical, body.resizing-vertical *, body.resizing-vertical *::after, body.resizing-vertical *::before {
  cursor: col-resize !important;
}
body.resizing-horizontal, body.resizing-horizontal *, body.resizing-horizontal *::after, body.resizing-horizontal *::before {
  cursor: row-resize !important;
}
.monaco-editor .monaco-scrollable-element .scrollbar .slider {
  background: rgba(0, 0, 0, 0.22);
  border-radius: 6px;
  transition: 100ms ease;
}
.monaco-editor .monaco-scrollable-element .scrollbar .slider:hover {
  background: rgba(0, 0, 0, 0.5);
}
.monaco-editor .monaco-scrollable-element .scrollbar.horizontal .slider {
  height: 7px !important;
  top: 3.5px !important;
}
.monaco-editor .monaco-scrollable-element .scrollbar.horizontal .slider:hover {
  height: 10px !important;
  top: 2px !important;
}
.monaco-editor .monaco-scrollable-element .scrollbar.vertical .slider {
  left: 3.5px !important;
  width: 7px !important;
}
.monaco-editor .monaco-scrollable-element .scrollbar.vertical .slider:hover {
  left: 2px !important;
  width: 10px !important;
}
.app {
  display: flex;
  height: 100%;
  position: relative;
  width: 100%;
}
.sidebar {
  background: #F8F8F8;
  border-right: 1px solid #D0D7DE;
  display: flex;
  flex-direction: column;
  padding: 8px 4px 8px 5px;
  row-gap: 4px;
}
.sidebar__action {
  align-items: center;
  appearance: none;
  background: transparent;
  border: none;
  border-radius: 4px;
  color: #6C707E;
  display: inline-flex;
  font-size: 16px;
  justify-content: center;
  padding: 5px 0;
  transition: background 130ms ease;
  width: 30px;
}
.sidebar__action:disabled {
  opacity: 0.5;
}
.sidebar__action:not(:disabled) {
  cursor: pointer;
}
.sidebar__action:not(:disabled):hover {
  background: rgba(184, 184, 184, 0.3);
}
.sidebar__action.sidebar__action--blue {
  color: #599E5E;
}
.editors {
  display: flex;
  width: calc(100% - 40px);
}
.editor {
  height: 100%;
  width: 100%;
}
.editor__wrapper {
  display: inline-flex;
  flex-direction: column;
  height: 100%;
  padding: 4px 0 0;
  position: relative;
}
.separator {
  position: relative;
  user-select: none;
}
.separator::after {
  content: '';
  position: absolute;
  transition: 100ms ease;
  z-index: 12;
}
.separator::before {
  background: #D0D7DE;
  content: '';
  position: absolute;
  transition: 100ms ease;
  z-index: 11;
}
.separator.active::after, .separator:hover::after {
  background: #0969DA;
}
.separator.vertical {
  cursor: col-resize;
  height: 100%;
  width: 0;
}
.separator.vertical::after {
  height: 100%;
  left: -3px;
  top: 0;
  width: 6px;
}
.separator.vertical::before {
  height: 100%;
  left: -0.5px;
  top: 0;
  width: 1px;
}
.separator.horizontal {
  cursor: row-resize;
  height: 0;
  width: 100%;
}
.separator.horizontal::after {
  height: 6px;
  left: 0;
  top: -3px;
  width: 100%;
}
.separator.horizontal::before {
  height: 1px;
  left: 0;
  top: -0.5px;
  width: 100%;
}
.output {
  background: #F8F8F8;
  display: flex;
  flex-direction: column;
  padding: 4px 8px 0 16px;
  row-gap: 8px;
  width: 100%;
}
.output__header {
  align-items: center;
  justify-content: space-between;
  display: flex;
  width: 100%;
}
.output__header-headline {
  color: #3B3B3B;
  font-family: sans-serif;
  font-size: 12px;
  font-weight: normal;
  line-height: 2.5;
  letter-spacing: 1px;
  text-transform: uppercase;
}
.output__header-actions {
  column-gap: 4px;
  display: inline-flex;
}
.output__header-action {
  appearance: none;
  align-items: center;
  background: transparent;
  border: none;
  border-radius: 4px;
  color: #6C707E;
  display: inline-flex;
  font-size: 16px;
  justify-content: center;
  padding: 3px 0;
  transition: background 130ms ease;
  width: 22px;
}
.output__header-action:not(:disabled) {
  cursor: pointer;
}
.output__header-action:not(:disabled):hover {
  background: rgba(184, 184, 184, 0.3);
}
.output__content-wrapper {
  flex-grow: 1;
  overflow: auto;
  padding-bottom: 8px;
}
.output__content {
  font-family: Menlo, Monaco, 'Courier New', monospace;
  font-size: 12px;
  font-weight: normal;
  line-height: 1.5;
  letter-spacing: 0;
  margin: 0;
}`

const commonJS = `var ws;
var editorsEl = document.getElementById('editors');
var editor1El = document.getElementById('editor-the');
var editor2El = document.getElementById('editor-c');
var separator1El = document.getElementById('separator1');
var separator2El = document.getElementById('separator2');
var outputEl = document.getElementById('output');
var outputCloseEl = document.getElementById('output-close');
var outputContentEl = document.getElementById('output-content');
var buildButtonEl = document.getElementById('build');
var runButtonEl = document.getElementById('run');
var editor1Ref = { current: null };
var editor2Ref = { current: null };
var separator1Controls = null;
var separator2Controls = null;
var initialBuild = true;
var initialEditor1State = localStorage.getItem('state1') !== null ? JSON.parse(localStorage.getItem('state1')) : {
  focus: false,
  scroll: 0,
  viewState: null
};
var initialEditor2State = localStorage.getItem('state2') !== null ? JSON.parse(localStorage.getItem('state2')) : {
  focus: false,
  scroll: 0,
  viewState: null
};
var editor1BuildValue = '';
function actionsDisabled () {
  return buildButtonEl.disabled || runButtonEl.disabled;
}
function disableActions () {
  buildButtonEl.disabled = true;
  runButtonEl.disabled = true;
}
function enableActions () {
  buildButtonEl.disabled = false;
  runButtonEl.disabled = false;
}
function applyEditorState (editorRef, state) {
  if (state.focus) {
    editorRef.current.focus();
  }
  editorRef.current.setScrollTop(state.scroll);
  editorRef.current.restoreViewState(state.viewState);
}
function freezeEditorState (editorRef) {
  return {
    focus: editorRef.current.hasTextFocus(),
    scroll: editorRef.current.getScrollTop(),
    viewState: editorRef.current.saveViewState()
  };
}
function formatCode (code) {
  var emptyLines = 0;
  var lines = code.split('\\n');
  var result = '';
  for (var i = 0; i < lines.length; i++) {
    var line = lines[i];
    if (line.trim().length === 0) {
      emptyLines++;
    } else {
      emptyLines = 0;
      result += line.trimEnd();
    }
    if (emptyLines <= 1) {
      result += '\\n';
    }
  }
  result = result.trimEnd();
  return result.length !== 0 ? result + '\\n' : result;
}
function connectWebSocket (initialConnect) {
  ws = new WebSocket('ws://' + location.host + '/ws');
  ws.onopen = function () {
    if (initialConnect !== true) {
      location.reload();
    }
  };
  ws.onclose = function () {
    setTimeout(connectWebSocket, 1000);
  };
  ws.onerror = function (err) {
    ws.close();
  };
  ws.addEventListener('message', function (e) {
    var payload;
    try {
      payload = JSON.parse(e.data);
    } catch (err) {
      return;
    }
    switch (payload.action) {
      case 'build-output': {
        editor2Ref.current.getModel().setValue(payload.body);
        if (initialBuild) {
          editor2Ref.current.setScrollTop(initialEditor2State.scroll);
          editor2Ref.current.restoreViewState(initialEditor2State.viewState);
          editorsEl.style.opacity = '';
          initialBuild = false;
        }
        enableActions();
        break;
      }
      case 'rebuild': {
        debouncedBuildWrapped(true);
        break;
      }
      case 'run-output': {
        showOutput(payload.body);
        enableActions();
        break;
      }
    }
  });
}
connectWebSocket(true);
function initSeparator (id, direction, separatorEl, firstEl, secondEl) {
  var isVertical = direction === 'vertical';
  var parent = firstEl.parentNode;
  var surfaceSize = isVertical ? parent.offsetWidth : parent.offsetHeight;
  var initialClientPos;
  var initialFirstSize;
  var initialSecondSize;
  function separate (at) {
    localStorage.setItem('separator' + id, at);
    if (isVertical) {
      firstEl.style.width = at + '%';
      secondEl.style.width = (100 - at) + '%';
    } else {
      firstEl.style.height = at + '%';
      secondEl.style.height = (100 - at) + '%';
    }
    editor1Ref.current.layout();
    editor2Ref.current.layout();
  }
  function handleMouseMove (e) {
    var clientPos = isVertical ? e.clientX : e.clientY;
    var delta = Math.min(Math.max(clientPos - initialClientPos, -initialFirstSize), initialSecondSize);
    separate(Math.round((initialFirstSize + delta) * 100 / surfaceSize));
  }
  var throttledMouseMoveHandler = _.throttle(handleMouseMove, 15);
  function handleMouseUp () {
    document.removeEventListener('mousemove', throttledMouseMoveHandler);
    document.removeEventListener('mouseup', handleMouseUp);
    separatorEl.classList.remove('active');
    document.body.classList.remove('resizing-' + direction);
  }
  function handleMouseDown (e) {
    initialClientPos = isVertical ? e.clientX : e.clientY;
    initialFirstSize = isVertical ? firstEl.offsetWidth : firstEl.offsetHeight;
    initialSecondSize = isVertical ? secondEl.offsetWidth : secondEl.offsetHeight;
    separatorEl.classList.add('active');
    document.body.classList.add('resizing-' + direction);
    document.addEventListener('mousemove', throttledMouseMoveHandler);
    document.addEventListener('mouseup', handleMouseUp);
  }
  separatorEl.addEventListener('mousedown', handleMouseDown);
  if (separatorEl.style.display !== 'none') {
    separate(parseInt(localStorage.getItem('separator' + id)) || 50);
  }
  return {
    hide: function () {
      separatorEl.style.display = 'none';
      if (isVertical) {
        firstEl.style.width = '';
      } else {
        firstEl.style.height = '';
      }
      editor1Ref.current.layout();
      editor2Ref.current.layout();
    },
    show: function () {
      separatorEl.style.display = '';
      separate(parseInt(localStorage.getItem('separator' + id)) || 50);
    }
  };
}
function handleChangeStateFactory (id, editorRef) {
  return function handleChangeState () {
    localStorage.setItem('state' + id, JSON.stringify(freezeEditorState(editorRef)));
  }
}
function registerEditorHandlers (editorRef, handler) {
  editorRef.current.onDidBlurEditorText(handler);
  editorRef.current.onDidChangeCursorPosition(handler);
  editorRef.current.onDidChangeCursorSelection(handler);
  editorRef.current.onDidFocusEditorText(handler);
  editorRef.current.onDidScrollChange(handler);
}
function hideOutput () {
  outputContentEl.innerText = '';
  outputEl.style.display = 'none';
  separator2Controls.hide();
}
function showOutput (content) {
  outputContentEl.innerText = content;
  outputEl.style.display = '';
  separator2Controls.show();
  outputContentEl.parentNode.scrollTop = outputContentEl.parentNode.scrollHeight;
}
function actionBuild () {
  var code = editor1Ref.current.getModel().getValue();
  localStorage.setItem('code', code);
  ws.send(JSON.stringify({
    action: 'build',
    body: code
  }));
}
function actionRun () {
  disableActions();
  ws.send(JSON.stringify({
    action: 'run',
    body: editor1Ref.current.getModel().getValue()
  }));
}
var debouncedEditor1StateChangeHandler = _.debounce(handleChangeStateFactory(1, editor1Ref), 100);
var debouncedEditor2StateChangeHandler = _.debounce(handleChangeStateFactory(2, editor2Ref), 100);
var debouncedBuild = _.debounce(actionBuild, 1000);
function debouncedBuildWrapped (force) {
  if (!(force === true)) {
    var code = editor1Ref.current.getModel().getValue();
    if (editor1BuildValue === code && !actionsDisabled()) {
      return;
    }
    editor1BuildValue = code;
  }
  disableActions();
  return debouncedBuild();
}
disableActions();
require.config({
  paths: {
    vs: 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.39.0/min/vs'
  }
});
require(['vs/editor/editor.main'], function () {
  monaco.languages.register({ id: 'the' });
  monaco.languages.setMonarchTokensProvider('the', {
    defaultToken: 'invalid',
    tokenPostfix: '.the',
    keywords: [
      'any',    'continue',
      'bool',   'elif',
      'byte',   'else',
      'char',   'enum',
      'float',  'export',
      'f32',    'false',
      'f64',    'finally',
      'int',    'fn',
      'i8',     'from',
      'i16',    'if',
      'i32',    'import',
      'i64',    'is',
      'str',    'loop',
      'u8',     'main',
      'u16',    'mut',
      'u32',    'nil',
      'u64',    'obj',
      'void',   'ref',
      'as',     'return',
      'async',  'throw',
      'await',  'true',
      'break',  'try',
      'catch',  'type',
      'const'
    ],
    operators: [
      '<=', '>=', '==', '!=', '=>',
      '+', '-', '*', '/', '%',
      '++', '--', '<<', '>>',
      '&', '|', '^', '!', '~',
      '&&', '||', '?', ':',
      '=', '+=', '-=', '*=', '/=', '%=', '<<=', '>>=', '&=', '|=', '^='
    ],
    escapes: /\\\\[0fnrtv"'\\\\]/,
    digits: /\\d+/,
    tokenizer: {
      root: [
        [/[{}]/, 'delimiter.bracket'],
        { include: 'common' }
      ],
      common: [
        [/[A-Za-z_]\\w*/, {
          cases: {
            '@keywords': 'keyword',
            '@default': 'identifier'
          }
        }],
        { include: '@whitespace' },
        [/[()[\\]]/, '@brackets'],
        [/!(?=([^=]|$))/, 'delimiter'],
        [/[=><!~?:&|+\\-*/^%]+/, {
          cases: {
            '@operators': 'delimiter',
            '@default': ''
          }
        }],
        [/[;,.]/, 'delimiter'],
        [/(@digits)[eE]([-+]?(@digits))?/, 'number.float'],
        [/(@digits)\\.(@digits)([eE][-+]?(@digits))?/, 'number.float'],
        [/0[xX]([[0-9a-fA-F]+)/, 'number.hex'],
        [/0[oO]([0-7]+)/, 'number.octal'],
        [/0[bB]([0-1]+)/, 'number.binary'],
        [/@digits/, 'number'],
        [/'([^'\\\\]|\\\\.)*$/, 'string.invalid'],
        [/"/, 'string', '@stringDouble'],
        [/'/, 'string', '@stringSingle']
      ],
      whitespace: [
        [/[ \\t\\r\\n]+/, ''],
        [/\\/\\*/, 'comment', '@comment'],
        [/\\/\\/.*$/, 'comment']
      ],
      comment: [
        [/[^/*]+/, 'comment'],
        [/\\*\\//, 'comment', '@pop'],
        [/[/*]/, 'comment']
      ],
      stringDouble: [
        [/[^\\\\"]+/, 'string'],
        [/@escapes/, 'string.escape'],
        [/\\\\./, 'string.escape.invalid'],
        [/"/, 'string', '@pop']
      ],
      stringSingle: [
        [/[^\\\\']+/, 'string'],
        [/@escapes/, 'string.escape'],
        [/\\\\./, 'string.escape.invalid'],
        [/'/, 'string', '@pop']
      ],
      bracketCounting: [
        [/\\{/, 'delimiter.bracket', '@bracketCounting'],
        [/}/, 'delimiter.bracket', '@pop'],
        { include: 'common' }
      ]
    }
  });
  var editorOptions = {
    minimap: {
      enabled: false
    },
    scrollBeyondLastLine: false,
    contextmenu: false,
    codeLens: false,
    tabSize: 2,
    overviewRulerLanes: 0,
    scrollbar: {
      useShadows: false,
      vertical: 'visible',
      horizontal: 'auto',
      verticalScrollbarSize: 14,
      horizontalScrollbarSize: 14
    }
  };
  editor1Ref.current = monaco.editor.create(editor1El, Object.assign(editorOptions, {
    value: localStorage.getItem('code') || 'fn test () {\\n}\\n\\nmain {\\n  test()\\n}',
    language: 'the'
  }));
  editor2Ref.current = monaco.editor.create(editor2El, Object.assign(editorOptions, {
    value: '',
    language: 'c',
    readOnly: true,
    domReadOnly: true
  }));
  document.addEventListener('keydown', function (e) {
    if (e.key === 's' && (navigator.platform.toUpperCase().indexOf('MAC') >= 0 ? e.metaKey : e.ctrlKey)) {
      e.preventDefault();
      var editor1State = freezeEditorState(editor1Ref);
      var code = editor1Ref.current.getModel().getValue();
      var formattedCode = formatCode(code);
      if (formattedCode !== code) {
        localStorage.setItem('code', formattedCode);
        editor1Ref.current.getModel().setValue(formattedCode);
        applyEditorState(editor1Ref, editor1State);
      }
    }
  }, false);
  buildButtonEl.addEventListener('click', function () {
    debouncedBuildWrapped(true);
  });
  runButtonEl.addEventListener('click', actionRun);
  outputCloseEl.addEventListener('click', hideOutput);
  separator1Controls = initSeparator(1, 'vertical', separator1El, editor1El.parentNode, editor2El.parentNode);
  separator2Controls = initSeparator(2, 'horizontal', separator2El, editor1El, outputEl);
  registerEditorHandlers(editor1Ref, debouncedEditor1StateChangeHandler);
  editor1Ref.current.onDidChangeModelContent(debouncedBuildWrapped);
  editor1Ref.current.onDidChangeCursorPosition(debouncedBuildWrapped);
  editor1Ref.current.onDidChangeCursorSelection(debouncedBuildWrapped);
  registerEditorHandlers(editor2Ref, debouncedEditor2StateChangeHandler);
  applyEditorState(editor1Ref, initialEditor1State);
  actionBuild();
});`

class WebSocketServer extends EventEmitter {
  GUID = '258EAFA5-E914-47DA-95CA-C5AB0DC85B11'
  OPCODES = {
    text: 0x01,
    close: 0x08
  }

  clients = new Set()

  constructor (server) {
    super()
    server.on('upgrade', this.handleUpgrade.bind(this))
  }

  broadcast (data) {
    for (const client of this.clients) {
      this.send(client, data)
    }
  }

  generateAcceptValue (acceptKey) {
    return crypto.createHash('sha1').update(acceptKey + this.GUID, 'binary').digest('base64')
  }

  handle (req, res) {
    res.setHeader('content-type', 'text/plain')
    res.setHeader('upgrade', 'WebSocket')
    res.writeHead(426)
    res.end('Upgrade Required')
  }

  handleData (socket, buf) {
    const message = this.parse(socket, buf)

    if (message) {
      this.emit('message', socket, message)
    } else if (message === null) {
      socket.destroy()
    }
  }

  handleUpgrade (req, socket) {
    if (req.headers.upgrade !== 'websocket') {
      socket.end('HTTP/1.1 400 Bad Request')
      return
    }

    const acceptKey = req.headers['sec-websocket-key']
    const acceptValue = this.generateAcceptValue(acceptKey)

    const responseHeaders = [
      'HTTP/1.1 101 Switching Protocols',
      'Upgrade: WebSocket',
      'Connection: Upgrade',
      `Sec-WebSocket-Accept: ${acceptValue}`
    ]

    socket.write(responseHeaders.concat('\r\n').join('\r\n'))
    socket.on('data', this.handleData.bind(this, socket))
    this.clients.add(socket)

    socket.on('error', () => {
      socket.destroy()
      this.clients.delete(socket)
    })

    socket.on('close', () => {
      socket.destroy()
      this.clients.delete(socket)
    })
  }

  parse (socket, buf) {
    const firstByte = buf.readUInt8(0)
    const opCode = firstByte & 0x0F

    if (opCode === this.OPCODES.close) {
      socket.emit('close')
      return null
    } else if (opCode !== this.OPCODES.text) {
      return
    }

    const secondByte = buf.readUInt8(1)
    const payloadLength = secondByte & 0x7F
    const offset = payloadLength === 126 ? 4 : payloadLength === 127 ? 10 : 2
    const isMasked = !!((secondByte >>> 7) & 0x01)

    if (isMasked) {
      const maskingKey = buf.readUInt32BE(offset)
      return this.unmask(buf.subarray(offset + 4), maskingKey).toString('utf-8')
    } else {
      return buf.subarray(offset).toString('utf-8')
    }
  }

  send (socket, data) {
    if (socket.destroyed) {
      return
    }

    const payload = data.toString()
    const payloadByteLength = Buffer.byteLength(payload)
    const payloadBytesOffset = payloadByteLength > 0xFFFF ? 10 : payloadByteLength > 125 ? 4 : 2
    const payloadLength = payloadByteLength > 0xFFFF ? 127 : payloadByteLength > 125 ? 126 : payloadByteLength
    const buffer = Buffer.alloc(payloadBytesOffset + payloadByteLength)

    buffer.writeUInt8(0x81, 0)
    buffer[1] = payloadLength

    if (payloadLength === 126) {
      buffer.writeUInt16BE(payloadByteLength, 2)
    } else if (payloadByteLength === 127) {
      buffer.writeBigUInt64BE(BigInt(payloadByteLength), 2)
    }

    buffer.write(payload, payloadBytesOffset)
    socket.write(buffer)
  }

  unmask (payload, maskingKey) {
    const result = Buffer.alloc(payload.byteLength)

    for (let i = 0, j = 0; i < payload.byteLength; i++, j = i % 4) {
      const shift = j === 3 ? 0 : (3 - j) << 3
      const mask = (shift === 0 ? maskingKey : maskingKey >>> shift) & 0xFF
      result.writeUInt8(mask ^ payload.readUInt8(i), i)
    }

    return result
  }
}

function handleGetHome (req, res) {
  res.setHeader('content-type', 'text/html')
  res.writeHead(200)
  res.end(homeHTML)
}

function handleGetCommonJS (req, res) {
  res.setHeader('cache-control', 'public, max-age=31536000')
  res.setHeader('content-type', 'text/javascript')
  res.writeHead(200)
  res.end(commonJS)
}

function handleGetStyleCSS (req, res) {
  res.setHeader('cache-control', 'public, max-age=31536000')
  res.setHeader('content-type', 'text/css')
  res.writeHead(200)
  res.end(styleCSS)
}

function handleAll (req, res) {
  res.setHeader('content-type', 'text/html')
  res.writeHead(404)
  res.end('404 Not Found')
}

async function enhanceRequest (req) {
  return await new Promise((resolve) => {
    const [route = '/', search = ''] = req.url.split('?')
    let body = ''

    req.on('data', (chunk) => {
      body += chunk
    })

    req.on('end', () => {
      req.body = body
      req.originalUrl = req.url
      req.url = route
      req.query = new URLSearchParams(search)

      resolve()
    })
  })
}

const server = http.createServer(router)
const ws = new WebSocketServer(server)

async function router (req, res) {
  await enhanceRequest(req)

  switch (`${req.method} ${req.url}`) {
    case 'GET /': return handleGetHome(req, res)
    case 'GET /common.js': return handleGetCommonJS(req, res)
    case 'GET /style.css': return handleGetStyleCSS(req, res)
    case 'GET /ws': return ws.handle(req, res)
    default: return handleAll(req, res)
  }
}

async function fsWatch (fileName) {
  return new Promise((resolve) => {
    const ac = new AbortController()

    fs.watch(fileName, {
      signal: ac.signal
    }, (eventType) => {
      ac.abort()
      resolve(eventType)
    })
  })
}

server.listen(8080, 'localhost', () => {
  (async () => {
    let shouldRebuild = false

    while (true) {
      try {
        await fsPromises.access(appBinaryPath)
      } catch {
        continue
      }

      if (shouldRebuild) {
        ws.broadcast(JSON.stringify({
          action: 'rebuild'
        }))

        shouldRebuild = false
      }

      const result = await fsWatch(appBinaryPath)

      if (result === 'rename') {
        shouldRebuild = true
      }
    }
  })()

  ws.on('message', async (socket, message) => {
    let payload

    try {
      payload = JSON.parse(message)
    } catch {
      return
    }

    switch (payload.action) {
      case 'build': {
        const { filePath } = await tmpFile(payload.body)

        try {
          const { stderr, stdout } = await exec(`${appBinaryPath} codegen ${filePath}`, {
            env: {
              THE_DEPS_DIR: appDepsDir,
              PATH: process.env.PATH
            },
            timeout: 3e4
          })

          ws.send(socket, JSON.stringify({
            action: 'build-output',
            body: (stdout + stderr).replaceAll(filePath, '/app')
          }))
        } finally {
          await fsPromises.rm(filePath, { force: true })
        }

        break
      }
      case 'run': {
        const { filePath, outputPath } = await tmpFile(payload.body)

        try {
          const { stderr: stderrCompile } = await exec(`${appBinaryPath} ${filePath} --output=${outputPath}`, {
            env: {
              THE_DEPS_DIR: appDepsDir,
              PATH: process.env.PATH
            },
            timeout: 3e4
          })

          if (stderrCompile !== '') {
            ws.send(socket, JSON.stringify({
              action: 'run-output',
              body: stderrCompile.replaceAll(filePath, '/app')
            }))

            return
          }

          const { code, stderr, stdout } = await exec(outputPath, {
            env: {
              PATH: process.env.PATH
            },
            timeout: 3e4
          })

          ws.send(socket, JSON.stringify({
            action: 'run-output',
            body: (stdout + stderr).replaceAll(filePath, '/app') +
              'Process finished with exit code ' + code
          }))
        } finally {
          await fsPromises.rm(outputPath + '.dSYM', { recursive: true, force: true })
          await fsPromises.rm(outputPath + '.ilk', { force: true })
          await fsPromises.rm(outputPath + '.pdb', { force: true })
          await fsPromises.rm(outputPath, { force: true })
          await fsPromises.rm(filePath, { force: true })
        }

        break
      }
    }
  })
})
