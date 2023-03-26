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

const fs = require('fs')
const path = require('path')

const TESTS_DIR = path.join(__dirname, '..', 'test')
const VALID_DIRS = ['parser', 'ast', 'codegen']

let hasErrors = false

function ucfirst (s) {
  return s.slice(0, 1).toUpperCase() + s.slice(1)
}

function validateTestsRegistered (dir) {
  const dirPath = path.join(TESTS_DIR, dir + '-test')
  const testPath = path.join(TESTS_DIR, ucfirst(dir) + 'Test.cpp')
  const tests = fs.readdirSync(dirPath)
  const content = fs.readFileSync(testPath, 'utf8')

  for (const test of tests) {
    if (test.startsWith('.')) {
      continue
    }

    const testName = '"' + test.slice(0, -4) + '"'

    if (!content.includes(testName)) {
      console.error(`Error: test ${testName} doesn't exists`)
      hasErrors = true
    }
  }
}

function main () {
  if (!VALID_DIRS.includes(process.argv[2])) {
    VALID_DIRS.forEach(validateTestsRegistered)
  } else {
    validateTestsRegistered(process.argv[2])
  }

  if (hasErrors) {
    process.exit(1)
  }
}

main()
