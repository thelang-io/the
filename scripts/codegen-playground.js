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
        code: err?.code ?? 0,
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
  width: calc(100% - 38px);
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
  background: #D0D7DE;
  position: relative;
  user-select: none;
}
.separator::after {
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
  width: 1px;
}
.separator.vertical::after {
  height: 100%;
  left: -2.5px;
  top: 0;
  width: 6px;
}
.separator.horizontal {
  cursor: row-resize;
  height: 1px;
  width: 100%;
}
.separator.horizontal::after {
  height: 6px;
  left: 0;
  top: -2.5px;
  width: 100%;
}
.output {
  background: #F8F8F8;
  display: flex;
  flex-direction: column;
  padding: 4px 8px 8px 16px;
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
}
.output__content {
  font-family: Menlo, Monaco, 'Courier New', monospace;
  font-size: 12px;
  font-weight: normal;
  line-height: 1.5;
  letter-spacing: 0;
  margin: 0;
}`

const commonJS = `var editorsEl = document.getElementById('editors');
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
      firstEl.style.width = 'calc(' + at + '% - 0.5px)';
      secondEl.style.width = 'calc(' + (100 - at) + '% - 0.5px)';
    } else {
      firstEl.style.height = 'calc(' + at + '% - 0.5px)';
      secondEl.style.height = 'calc(' + (100 - at) + '% - 0.5px)';
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
    },
    show: function () {
      separatorEl.style.display = '';
      separate(parseInt(localStorage.getItem('separator' + id)) || 50);
    }
  };
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
      editorsEl.style.opacity = '';
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
    await fs.rm(filePath, { force: true })
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

    const { code, stderr, stdout } = await exec(outputPath, {
      env: {
        PATH: process.env.PATH
      },
      timeout: 3e4
    })

    let output = (stderr !== '' ? stderr : stdout).replaceAll(filePath, '/app')
    output += 'Process finished with exit code ' + code

    res.setHeader('content-type', 'text/plain')
    res.writeHead(200)
    res.end(output)
  } catch {
    res.setHeader('content-type', 'text/html')
    res.writeHead(500)
    res.end('Internal Server Error')
  } finally {
    await fs.rm(outputPath + '.dSYM', { recursive: true, force: true })
    await fs.rm(outputPath + '.ilk', { force: true })
    await fs.rm(outputPath + '.pdb', { force: true })
    await fs.rm(outputPath, { force: true })
    await fs.rm(filePath, { force: true })
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
