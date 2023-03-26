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
