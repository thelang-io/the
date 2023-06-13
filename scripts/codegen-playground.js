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

const childProcess = require('child_process')
const crypto = require('crypto')
const fs = require('fs/promises')
const http = require('http')
const path = require('path')

async function exec (command, options) {
  return await new Promise((resolve) => {
    childProcess.exec(command, options, (err, stdout, stderr) => {
      resolve({
        code: err?.code ?? 1,
        stderr,
        stdout
      })
    })
  })
}

const appBinaryPath = 'build/the'
const appDepsDir = process.env.DEPS_DIR
const buildTime = Date.now()
const doctypeHTML = '<!doctype html>'
const homeHTML = doctypeHTML + `<html lang="en">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
    <link rel="icon" href="https://docs.thelang.io/favicon.ico" type="image/x-icon" />
    <title>The Playground</title>
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.39.0/min/vs/editor/editor.main.min.css" integrity="sha512-/hTKJ6YcK7JBJmV7HzjxM7LDShlTlamVMPdB0CaQRV5NeVS+ZVx8MogcT8Rw0DMRVGT7rNE+mDSc2QEVuJNdNA==" crossorigin="anonymous" referrerpolicy="no-referrer" />
    <link rel="stylesheet" href="/style.css?t=${buildTime}" />
  </head>
  <body>
    <div class="app">
      <div class="header">
        <button class="header__action" id="render" type="button">
          <svg viewBox="0 0 512 512" xmlns="http://www.w3.org/2000/svg"><path d="M463.5 224H472C485.3 224 496 213.3 496 200V72.0001C496 62.3001 490.2 53.5001 481.2 49.8001C472.2 46.1001 461.9 48.1001 455 55.0001L413.4 96.6001C325.8 10.1001 184.7 10.4001 97.6001 97.6001C10.1001 185.1 10.1001 326.9 97.6001 414.4C185.1 501.9 326.9 501.9 414.4 414.4C426.9 401.9 426.9 381.6 414.4 369.1C401.9 356.6 381.6 356.6 369.1 369.1C306.6 431.6 205.3 431.6 142.8 369.1C80.3001 306.6 80.3001 205.3 142.8 142.8C205 80.6001 305.5 80.3001 368.1 141.8L327 183C320.1 189.9 318.1 200.2 321.8 209.2C325.5 218.2 334.3 224 344 224H463.5Z" fill="currentColor" /></svg>
          <span>Refresh</span>
        </button>
      </div>
      <div class="editors" id="editors">
        <div class="editor" id="editor-the"></div>
        <div class="separator" id="separator"></div>
        <div class="editor" id="editor-c"></div>
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
.monaco-editor .monaco-scrollable-element .scrollbar .slider {
  background: #C9C9C9;
  border-radius: 4px;
}
.monaco-editor .monaco-scrollable-element .scrollbar.horizontal .slider {
  height: 7px !important;
  top: 3.5px !important;
}
.monaco-editor .monaco-scrollable-element .scrollbar.vertical .slider {
  left: 3.5px !important;
  width: 7px !important;
}
.app {
  display: flex;
  flex-direction: column;
  height: 100%;
  width: 100%;
}
.header {
  border-bottom: solid 1px #EBECF0;
  display: flex;
  justify-content: flex-end;
  padding: 16px 32px;
}
.header__action {
  align-items: center;
  appearance: none;
  background: #1473E6;
  border: none;
  border-radius: 16px;
  color: #FFFFFF;
  column-gap: 8px;
  display: inline-flex;
  font-family: Source Sans Pro, -apple-system, BlinkMacSystemFont, Segoe UI, Roboto, sans-serif;
  font-size: 14px;
  font-style: normal;
  font-weight: 700;
  letter-spacing: normal;
  line-height: 1.3;
  min-width: 80px;
  padding: 7px 16px;
  transition: background 130ms ease;
}
.header__action:hover {
  background: #0D66D0;
}
.header__action:disabled {
  opacity: 0.5;
}
.header__action:not(:disabled) {
  cursor: pointer;
}
.header__action > svg {
  height: 16px;
  width: 16px;
}
.editors {
  display: flex;
  flex-grow: 1;
}
.editor {
  height: 100%;
}
.separator {
  cursor: col-resize;
  height: 100%;
  position: relative;
  user-select: none;
  width: 6px;
}
.separator::after {
  background: #EBECF0;
  content: '';
  height: 100%;
  left: 2.5px;
  position: absolute;
  top: 0;
  user-select: none;
  width: 1px;
}`

const commonJS = `var editorsEl = document.getElementById('editors');
var editor1El = document.getElementById('editor-the');
var editor2El = document.getElementById('editor-c');
var separatorEl = document.getElementById('separator');
var renderButtonEl = document.getElementById('render');
var editor1Ref = { current: null };
var editor2Ref = { current: null };
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
var editor1RenderValue = '';
renderButtonEl.disabled = true;
function applyEditorState (editorRef, state) {
  if (state.focus) {
    editorRef.current.focus();
  }
  editorRef.current.setScrollTop(state.scroll);
  editorRef.current.restoreViewState(state.viewState);
}
function initSeparator () {
  var surfaceWidth = editorsEl.offsetWidth;
  var halfSeparator = separatorEl.offsetWidth / 2;
  var initialClientX;
  var initialEditor1Width;
  var initialEditor2Width;
  function separate (at) {
    localStorage.setItem('separator', at);
    editor1El.style.width = 'calc(' + at + '% - ' + halfSeparator + 'px)';
    editor2El.style.width = 'calc(' + (100 - at) + '% - ' + halfSeparator + 'px)';
    editor1Ref.current.layout();
    editor2Ref.current.layout();
  }
  function handleMouseMove (e) {
    var deltaX = Math.min(Math.max(e.clientX - initialClientX, -initialEditor1Width), initialEditor2Width);
    separate(Math.round((initialEditor1Width + deltaX) * 100 / surfaceWidth));
  }
  var throttledMouseMoveHandler = _.throttle(handleMouseMove, 15);
  function handleMouseUp () {
    document.removeEventListener('mousemove', throttledMouseMoveHandler);
    document.removeEventListener('mouseup', handleMouseUp);
  }
  function handleMouseDown (e) {
    initialClientX = e.clientX;
    initialEditor1Width = editor1El.offsetWidth;
    initialEditor2Width = editor2El.offsetWidth;
    document.addEventListener('mousemove', throttledMouseMoveHandler);
    document.addEventListener('mouseup', handleMouseUp);
  }
  separatorEl.addEventListener('mousedown', handleMouseDown);
  separate(parseInt(localStorage.getItem('separator')) || 50);
}
function handleChangeStateFactory (id, editorRef) {
  return function handleChangeState () {
    localStorage.setItem('state' + id, JSON.stringify({
      focus: editorRef.current.hasTextFocus(),
      scroll: editorRef.current.getScrollTop(),
      viewState: editorRef.current.saveViewState()
    }));
  }
}
function registerEditorHandlers (editorRef, handler) {
  editorRef.current.onDidBlurEditorText(handler);
  editorRef.current.onDidChangeCursorPosition(handler);
  editorRef.current.onDidChangeCursorSelection(handler);
  editorRef.current.onDidFocusEditorText(handler);
  editorRef.current.onDidScrollChange(handler);
}
function render (initialRender) {
  initialRender = initialRender === true;
  var code = editor1Ref.current.getModel().getValue();
  localStorage.setItem('code', code);
  fetch('/codegen', {
    method: 'POST',
    headers: {
      'content-type': 'text/plain'
    },
    body: code
  }).then(function (response) {
    return response.text();
  }).then(function (data) {
    editor2Ref.current.getModel().setValue(data);
    if (initialRender) {
      editor2Ref.current.setScrollTop(initialEditor2State.scroll);
      editor2Ref.current.restoreViewState(initialEditor2State.viewState);
    }
    renderButtonEl.disabled = false;
  });
}
var debouncedEditor1StateChangeHandler = _.debounce(handleChangeStateFactory(1, editor1Ref), 100);
var debouncedEditor2StateChangeHandler = _.debounce(handleChangeStateFactory(2, editor2Ref), 100);
var debouncedRender = _.debounce(render, 1000);
function debouncedRenderWrapped (force) {
  if (!(force === true)) {
    var code = editor1Ref.current.getModel().getValue();
    if (editor1RenderValue === code && !renderButtonEl.disabled) {
      return;
    }
    editor1RenderValue = code;
  }
  renderButtonEl.disabled = true;
  return debouncedRender();
}
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
  renderButtonEl.addEventListener('click', function () {
    debouncedRenderWrapped(true);
  });
  initSeparator();
  registerEditorHandlers(editor1Ref, debouncedEditor1StateChangeHandler);
  editor1Ref.current.onDidChangeModelContent(debouncedRenderWrapped);
  editor1Ref.current.onDidChangeCursorPosition(debouncedRenderWrapped);
  editor1Ref.current.onDidChangeCursorSelection(debouncedRenderWrapped);
  registerEditorHandlers(editor2Ref, debouncedEditor2StateChangeHandler);
  applyEditorState(editor1Ref, initialEditor1State);
  render(true);
});`

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

async function handlePostCodegen (req, res) {
  const buf = await crypto.randomBytes(3)
  const filepath = path.resolve(process.cwd(), `tmp-${buf.readUIntBE(0, 3)}`)
  const outputPath = `${filepath}.out`
  await fs.writeFile(filepath, req.body, 'utf8')

  try {
    const { stderr, stdout } = await exec(`${appBinaryPath} codegen ${filepath} --output=${outputPath}`, {
      env: {
        DEPS_DIR: appDepsDir,
        PATH: process.env.PATH
      },
      timeout: 3e4
    })

    let output

    if (stderr !== '') {
      output = stderr.replaceAll('/' + filepath, '')
    } else {
      output = stdout.replaceAll('/' + filepath, '')
    }

    res.setHeader('content-type', 'text/plain')
    res.writeHead(200)
    res.end(output)
  } catch {
    res.setHeader('content-type', 'text/html')
    res.writeHead(500)
    res.end('Internal Server Error')
  } finally {
    await fs.rm(filepath)
  }
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

async function router (req, res) {
  await enhanceRequest(req)

  switch (`${req.method} ${req.url}`) {
    case 'GET /': return handleGetHome(req, res)
    case 'GET /common.js': return handleGetCommonJS(req, res)
    case 'GET /style.css': return handleGetStyleCSS(req, res)
    case 'POST /codegen': return handlePostCodegen(req, res)
    default: return handleAll(req, res)
  }
}

const server = http.createServer(router)
server.listen(8080, 'localhost')
