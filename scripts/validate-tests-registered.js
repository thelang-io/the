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
