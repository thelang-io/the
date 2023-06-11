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
const doctypeHTML = '<!doctype html>'
const homeHTML = doctypeHTML + `<html lang="en">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.39.0/min/vs/editor/editor.main.min.css" integrity="sha512-/hTKJ6YcK7JBJmV7HzjxM7LDShlTlamVMPdB0CaQRV5NeVS+ZVx8MogcT8Rw0DMRVGT7rNE+mDSc2QEVuJNdNA==" crossorigin="anonymous" referrerpolicy="no-referrer" />
    <title>The Playground</title>
    <style>
      body, html {
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
      .editors {
        display: flex;
        height: 100%;
      }
      .editor {
        height: 100%;
        width: 65%;
      }
      .editor:first-of-type {
        width: 35%;
      }
    </style>
  </head>
  <body>
    <div class="editors">
      <div class="editor" id="editor-the"></div>
      <div class="editor" id="editor-c"></div>
    </div>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/lodash.js/4.17.21/lodash.min.js" integrity="sha512-WFN04846sdKMIP5LKNphMaWzU7YpMyCU245etK3g/2ARYbPK9Ub18eG+ljU96qKRCWh+quCY7yefSmlkQw1ANQ==" crossorigin="anonymous" referrerpolicy="no-referrer"></script>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.39.0/min/vs/loader.min.js" integrity="sha512-A+6SvPGkIN9Rf0mUXmW4xh7rDvALXf/f0VtOUiHlDUSPknu2kcfz1KzLpOJyL2pO+nZS13hhIjLqVgiQExLJrw==" crossorigin="anonymous" referrerpolicy="no-referrer"></script>
    <script async>
      require.config({
        paths: {
          vs: 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.39.0/min/vs'
        }
      });
      require(['vs/editor/editor.main'], function () {
        var editor1;
        var editor2;
        function render () {
          var code = editor1.getModel().getValue();
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
            editor2.getModel().setValue(data);
          });
        }
        editor1 = monaco.editor.create(document.getElementById('editor-the'), {
          value: localStorage.getItem('code') || 'fn test () {\\n}\\n\\nmain {\\n  test()\\n}',
          language: 'the',
          minimap: {
            enabled: false
          },
          scrollBeyondLastLine: false,
          contextmenu: false,
          codeLens: false,
          tabSize: 2
        });
        editor2 = monaco.editor.create(document.getElementById('editor-c'), {
          value: '',
          language: 'c',
          minimap: {
            enabled: false
          },
          readOnly: true,
          scrollBeyondLastLine: false,
          contextmenu: false,
          codeLens: false
        });
        editor1.onDidChangeModelContent(_.debounce(render, 1000));
        render();
      });
    </script>
  </body>
</html>`

function handleHome (req, res) {
  res.setHeader('content-type', 'text/html')
  res.writeHead(200)
  res.end(homeHTML)
}

async function handleCodegen (req, res) {
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

  switch (req.url) {
    case '/': return handleHome(req, res)
    case '/codegen': return handleCodegen(req, res)
    default: return handleAll(req, res)
  }
}

const server = http.createServer(router)
server.listen(8080, 'localhost')
