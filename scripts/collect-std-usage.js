const fs = require('fs')
const fsPromises = require('fs/promises')

const IGNORED_DIRECTORIES = [process.cwd() + '/build']

function traverseDirectory (dir = process.cwd()) {
  if (IGNORED_DIRECTORIES.includes(dir)) {
    return []
  }

  const files = []
  const entities = fs.readdirSync(dir)

  for (const entity of entities) {
    const entityPath = dir + '/' + entity
    const stat = fs.lstatSync(entityPath)

    if (entity.endsWith('.cpp') || entity.endsWith('.hpp')) {
      files.push(dir + '/' + entity)
    } else if (stat.isDirectory()) {
      files.push(...traverseDirectory(entityPath))
    }
  }

  return files
}

async function processFile (file) {
  const content = await fsPromises.readFile(file, { encoding: 'utf8' })
  const matches = content.match(/std::[\w:]+/g)
  return matches === null ? [] : [...matches]
}

async function main () {
  const files = traverseDirectory()
  const stdEntitiesRaw = await Promise.all(files.map(processFile))
  const stdEntities = Array.from(new Set(stdEntitiesRaw.flat())).sort()

  for (const stdEntity of stdEntities) {
    console.log(stdEntity)
  }
}

main().catch(console.error)
