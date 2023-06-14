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

const appBinaryPath = 'build/the'
const appDepsDir = process.env.DEPS_DIR
const buildTime = Date.now()

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

async function codegen (body) {
  const { filePath, outputPath } = await tmpFile(body)
  let result

  try {
    result = await exec(`${appBinaryPath} codegen ${filePath} --output=${outputPath}.out`, {
      env: {
        DEPS_DIR: appDepsDir,
        PATH: process.env.PATH
      },
      timeout: 3e4
    })
  } finally {
    await fs.rm(filePath)
  }

  return result
}

async function tmpFile (body) {
  const buf = await crypto.randomBytes(3)
  const filePath = path.resolve(process.cwd(), `tmp-${buf.readUIntBE(0, 3)}`)
  const outputPath = `${filePath}.out`
  await fs.writeFile(filePath, body, 'utf8')
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
      <div class="header">
        <button class="header__action" id="build" type="button">
          <i class="fa-solid fa-wrench"></i>
          <span>Build</span>
        </button>
        <button class="header__action header__action--blue" id="run" type="button">
          <i class="fa-solid fa-play"></i>
          <span>Run</span>
        </button>
      </div>
      <div class="editors" id="editors">
        <div class="editor" id="editor-the"></div>
        <div class="separator" id="separator"></div>
        <div class="editor" id="editor-c"></div>
      </div>
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
  position: relative;
  width: 100%;
}
.header {
  background: #F8F8F8;
  border: solid 1px #EBECF0;
  border-radius: 32px;
  column-gap: 8px;
  display: flex;
  justify-content: flex-end;
  margin: 8px;
  padding: 16px;
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
.header__action.header__action--blue {
  background: #599E5E;
}
.header__action.header__action--blue:hover {
  background: #508E54;
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
}
.output {
  background: #F8F8F8;
  border: solid 1px #EBECF0;
  border-radius: 32px;
  bottom: 8px;
  column-gap: 8px;
  display: flex;
  flex-direction: column;
  height: 40vh;
  left: 8px;
  padding: 16px;
  position: absolute;
  width: calc(100% - 16px);
  z-index: 11;
}
.output__header {
  align-items: flex-start;
  justify-content: space-between;
  display: flex;
  width: 100%;
}
.output__header-headline {
  color: #3B3B3B;
}
.output__header-actions {
}
.output__header-action {
}
.output__content-wrapper {
  flex-grow: 1;
  overflow-y: auto;
}
.output__content {
  margin: 0;
}`

const commonJS = `var editorsEl = document.getElementById('editors');
var editor1El = document.getElementById('editor-the');
var editor2El = document.getElementById('editor-c');
var separatorEl = document.getElementById('separator');
var outputEl = document.getElementById('output');
var outputCloseEl = document.getElementById('output-close');
var outputContentEl = document.getElementById('output-content');
var buildButtonEl = document.getElementById('build');
var runButtonEl = document.getElementById('run');
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
function hideOutput () {
  outputEl.style.display = 'none';
}
function showOutput (content) {
  outputContentEl.innerText = content;
  outputEl.style.display = '';
}
function actionBuild (initialBuild) {
  initialBuild = initialBuild === true;
  var code = editor1Ref.current.getModel().getValue();
  localStorage.setItem('code', code);
  fetch('/build', {
    method: 'POST',
    headers: {
      'content-type': 'text/plain'
    },
    body: code
  }).then(function (response) {
    return response.text();
  }).then(function (data) {
    editor2Ref.current.getModel().setValue(data);
    if (initialBuild) {
      editor2Ref.current.setScrollTop(initialEditor2State.scroll);
      editor2Ref.current.restoreViewState(initialEditor2State.viewState);
    }
    enableActions();
  });
}
function actionRun () {
  disableActions();
  var code = editor1Ref.current.getModel().getValue();
  fetch('/run', {
    method: 'POST',
    headers: {
      'content-type': 'text/plain'
    },
    body: code
  }).then(function (response) {
    return response.text();
  }).then(function (data) {
    showOutput(data);
    enableActions();
  });
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
outputCloseEl.addEventListener('click', function () {
  outputEl.style.display = 'none';
});
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
  buildButtonEl.addEventListener('click', function () {
    debouncedBuildWrapped(true);
  });
  runButtonEl.addEventListener('click', actionRun);
  initSeparator();
  registerEditorHandlers(editor1Ref, debouncedEditor1StateChangeHandler);
  editor1Ref.current.onDidChangeModelContent(debouncedBuildWrapped);
  editor1Ref.current.onDidChangeCursorPosition(debouncedBuildWrapped);
  editor1Ref.current.onDidChangeCursorSelection(debouncedBuildWrapped);
  registerEditorHandlers(editor2Ref, debouncedEditor2StateChangeHandler);
  applyEditorState(editor1Ref, initialEditor1State);
  actionBuild(true);
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

async function handlePostBuild (req, res) {
  const { filePath } = await tmpFile(req.body)

  try {
    const { stderr, stdout } = await exec(`${appBinaryPath} codegen ${filePath}`, {
      env: {
        DEPS_DIR: appDepsDir,
        PATH: process.env.PATH
      },
      timeout: 3e4
    })

    const output = (stderr !== '' ? stderr : stdout).replaceAll(filePath, '/app')

    res.setHeader('content-type', 'text/plain')
    res.writeHead(200)
    res.end(output)
  } catch {
    res.setHeader('content-type', 'text/html')
    res.writeHead(500)
    res.end('Internal Server Error')
  } finally {
    await fs.rm(filePath)
  }
}

async function handlePostRun (req, res) {
  const { filePath, outputPath } = await tmpFile(req.body)

  try {
    const { stderr: stderrCompile } = await exec(`${appBinaryPath} ${filePath} --output=${outputPath}`, {
      env: {
        DEPS_DIR: appDepsDir,
        PATH: process.env.PATH
      },
      timeout: 3e4
    })

    if (stderrCompile !== '') {
      const output = stderrCompile.replaceAll(filePath, '/app')
      res.setHeader('content-type', 'text/plain')
      res.writeHead(200)
      res.end(output)
      return
    }

    const { stderr, stdout } = await exec(outputPath, {
      env: {
        PATH: process.env.PATH
      },
      timeout: 3e4
    })

    const output = (stderr !== '' ? stderr : stdout).replaceAll(filePath, '/app')

    res.setHeader('content-type', 'text/plain')
    res.writeHead(200)
    res.end(output)
  } catch (err) {
  console.error(err)
    res.setHeader('content-type', 'text/html')
    res.writeHead(500)
    res.end('Internal Server Error')
  } finally {
    await fs.rm(outputPath)
    await fs.rm(filePath)
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
    case 'POST /build': return handlePostBuild(req, res)
    case 'POST /run': return handlePostRun(req, res)
    default: return handleAll(req, res)
  }
}

const server = http.createServer(router)
server.listen(8080, 'localhost')
