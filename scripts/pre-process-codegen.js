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
const os = require('os')
const path = require('path')

const srcDir = path.resolve(__dirname, '..', 'src')
const codegenDir = path.resolve(srcDir, 'codegen')

const banner = '/*!' + os.EOL +
  ' * Copyright (c) 2018 Aaron Delasy' + os.EOL +
  ' *' + os.EOL +
  ' * Licensed under the Apache License, Version 2.0 (the "License");' + os.EOL +
  ' * you may not use this file except in compliance with the License.' + os.EOL +
  ' * You may obtain a copy of the License at' + os.EOL +
  ' *' + os.EOL +
  ' *     http://www.apache.org/licenses/LICENSE-2.0' + os.EOL +
  ' *' + os.EOL +
  ' * Unless required by applicable law or agreed to in writing, software' + os.EOL +
  ' * distributed under the License is distributed on an "AS IS" BASIS,' + os.EOL +
  ' * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.' + os.EOL +
  ' * See the License for the specific language governing permissions and' + os.EOL +
  ' * limitations under the License.' + os.EOL +
  ' */' + os.EOL

const ENTITIES_THROWS_METADATA = {
  array_request_Header: true,
  array_request_Header_free: false,
  array_str: true,
  array_str_free: false,
  error_Error: false,
  error_Error_alloc: true,
  fs_Stats: false,
  fs_Stats_alloc: true,
  map_str_str: false,
  pair_str_str: false,
  request_Header: false,
  request_Header_alloc: true,
  request_Request: false,
  request_Response: false,
  request_Response_alloc: true,
  url_URL: false,
  url_URL_alloc: true,
  url_URL_free: false
}

async function main () {
  const metadata = parseMetadata()
  const entries = fs.readdirSync(codegenDir).filter(it => it.slice(-4) === '.cpp')
  const api = {}

  for (const entry of entries) {
    const content = fs.readFileSync(path.join(codegenDir, entry), 'utf8')
    const functionsContent = content.slice(content.indexOf(' = {' + os.EOL) + 4, content.indexOf('};' + os.EOL)).trim()
    const functions = functionsContent.split(',' + os.EOL + os.EOL).map(it => it.trim())

    for (const fn of functions) {
      let usesAdvancedEscaping = false
      const lines = fn.split(os.EOL).map(it => it.trim())
        .map((line) => {
          let result = line

          if (line.startsWith('R"(')) {
            result = result.slice(3)
          } else if (line.startsWith('R"~(')) {
            usesAdvancedEscaping = true
            result = result.slice(4)
          }
          if (line.endsWith(')" EOL')) {
            result = result.slice(0, -6)
          }
          if (line.endsWith(')~" EOL')) {
            usesAdvancedEscaping = true
            result = result.slice(0, -7)
          }

          return result
        })

      const fnContent = lines.join(os.EOL)
      const parsed = cParse(fnContent)

      const item = {
        name: parsed.name,
        decl: cParseDecl(parsed),
        def: fnContent,
        fileDependencies: [],
        dependencies: [],
        entityDependencies: [],
        throws: false,
        usesAdvancedEscaping
      }

      item.decl = apiItemEval(item, item.decl)
      item.def = apiItemEval(item, item.def)

      api[parsed.name] = item
    }
  }

  const keys = Object.keys(api).sort()

  for (let i = 0; i < keys.length; i++) {
    const key = keys[i]
    const metadataDependencies = []
    const metadataEntityDependencies = []

    for (const dependency of api[key].fileDependencies) {
      if (Object.prototype.hasOwnProperty.call(api, dependency)) {
        if (!metadataDependencies.includes(dependency)) {
          metadataDependencies.push(dependency)
        }
      } else if (Object.prototype.hasOwnProperty.call(metadata, dependency)) {
        for (const lib of metadata[dependency]) {
          if (!metadataDependencies.includes(lib)) {
            metadataDependencies.push(lib)
          }
        }
      } else {
        if (!metadataEntityDependencies.includes(dependency)) {
          metadataEntityDependencies.push(dependency)
        }
      }
    }

    api[key].dependencies = metadataDependencies
    api[key].entityDependencies = metadataEntityDependencies
  }

  while (true) {
    let hasChanges = false

    for (let i = 0; i < keys.length; i++) {
      const key = keys[i]

      if (api[key].throws) {
        continue
      }

      let throws = api[key].fileDependencies.includes('error_throw')

      if (!throws) {
        for (const dependency of api[key].fileDependencies) {
          if (Object.prototype.hasOwnProperty.call(metadata, dependency)) {
            continue
          }

          if (Object.prototype.hasOwnProperty.call(api, dependency)) {
            if (api[dependency].throws) {
              throws = true
              break
            }
          } else {
            if (!Object.prototype.hasOwnProperty.call(ENTITIES_THROWS_METADATA, dependency)) {
              throw new Error('Throw metadata does not exist for dependency `' + dependency + '`')
            }

            if (ENTITIES_THROWS_METADATA[dependency]) {
              throws = true
              break
            }
          }
        }
      }

      if (throws) {
        api[key].throws = throws
        hasChanges = true
      }
    }

    if (!hasChanges) {
      break
    }
  }

  let output = banner + os.EOL

  output += '#ifndef SRC_CODEGEN_API_HPP' + os.EOL
  output += '#define SRC_CODEGEN_API_HPP' + os.EOL + os.EOL
  output += '#include <map>' + os.EOL
  output += '#include <set>' + os.EOL
  output += '#include <string>' + os.EOL
  output += '#include "config.hpp"' + os.EOL + os.EOL
  output += 'struct CodegenAPIItem {' + os.EOL
  output += '  bool enabled;' + os.EOL
  output += '  std::string name;' + os.EOL
  output += '  std::string decl;' + os.EOL
  output += '  std::string def;' + os.EOL
  output += '  std::set<std::string> dependencies;' + os.EOL
  output += '  std::set<std::string> entityDependencies;' + os.EOL
  output += '  bool throws;' + os.EOL
  output += '};' + os.EOL + os.EOL
  output += 'extern inline const std::map<std::string, CodegenAPIItem> codegenAPI = {' + os.EOL

  for (let i = 0; i < keys.length; i++) {
    const key = keys[i]
    const item = api[key]
    const defLines = item.def.split(os.EOL)

    output += `  {"${item.name}", {` + os.EOL
    output += `    false,` + os.EOL
    output += `    "${item.name}",` + os.EOL
    output += `    "${item.decl}" EOL,` + os.EOL

    for (let i = 0; i < defLines.length; i++) {
      const line = defLines[i]
      const escapeStart = item.usesAdvancedEscaping ? 'R"~(' : 'R"('
      const escapeEnd = item.usesAdvancedEscaping ? ')~"' : ')"'

      output += '    ' + escapeStart + line + escapeEnd + ' EOL' + (i === defLines.length - 1 ? ',' : '') + os.EOL
    }

    output += `    {${item.dependencies.sort().map(it => '"' + it + '"').join(', ')}},` + os.EOL
    output += `    {${item.entityDependencies.sort().map(it => '"' + it + '"').join(', ')}},` + os.EOL
    output += '    ' + (item.throws ? 'true' : 'false') + os.EOL

    if (i !== keys.length - 1) {
      output += `  }},` + os.EOL + os.EOL
    } else {
      output += `  }}` + os.EOL
    }
  }

  output += '};' + os.EOL + os.EOL
  output += '#endif' + os.EOL

  fs.writeFileSync(path.join(srcDir, 'codegen-api.hpp'), output, 'utf8')
}

const CPARSER_ID = 0
const CPARSER_REPLACEMENT = 1
const CPARSER_WHITESPACE = 2
const CPARSER_OP_STAR = 3
const CPARSER_OP_LPAR = 4
const CPARSER_OP_RPAR = 5
const CPARSER_OP_COMMA = 6
const CPARSER_OP_DOT_DOT_DOT = 7

function extractDecl (s) {
  let result = ''

  for (let i = 0; i < s.length; i++) {
    if (s[i] == '\n' || (s[i] == '\r' && i + 1 < s.length && s[i + 1] == '\n')) {
      break
    } else {
      result += s[i]
    }
  }

  return result.slice(0, -2)
}

function tokenize (s) {
  let result = []

  for (let i = 0; i < s.length; i++) {
    if (/\s/.test(s[i])) {
      let val = s[i]

      while (i + 1 < s.length && /\s/.test(s[i + 1])) {
        val += s[++i]
      }

      result.push({type: CPARSER_WHITESPACE, val})
    } else if (s[i] == '_' && i + 1 < s.length && s[i + 1] == '{') {
      let val = s[i]

      while (i + 1 < s.length) {
        val += s[++i]

        if (s[i] == '}') {
          break
        }
      }

      result.push({type: CPARSER_REPLACEMENT, val})
    } else if (/[a-zA-Z]/.test(s[i]) || s[i] == '_') {
      let val = s[i]

      while (i + 1 < s.length && (/[a-zA-Z0-9]/.test(s[i + 1]) || s[i + 1] == '_' || s[i + 1] == '$')) {
        val += s[++i]
      }

      result.push({type: CPARSER_ID, val})
    } else if (s[i] == '.' && i + 2 < s.length && s[i + 1] == '.' && s[i + 2] == '.') {
      result.push({type: CPARSER_OP_DOT_DOT_DOT, val: "..."})
      i += 2
    } else if (s[i] == '(') {
      result.push({type: CPARSER_OP_LPAR, val: s[i]})
    } else if (s[i] == ')') {
      result.push({type: CPARSER_OP_RPAR, val: s[i]})
    } else if (s[i] == '*') {
      result.push({type: CPARSER_OP_STAR, val: s[i]})
    } else if (s[i] == ',') {
      result.push({type: CPARSER_OP_COMMA, val: s[i]})
    } else {
      throw new Error("unexpected token `" + s + "` in parser in position " + i)
    }
  }

  return result
}

function tkLookahead (tokens, i, type) {
  let j = i
  while (j + 1 < tokens.length && tokens[j + 1].type == CPARSER_WHITESPACE) j++
  return j + 1 < tokens.length && tokens[j + 1].type == type ? j : i
}

function tkBehind (tokens, i) {
  while (i != 0 && tokens[i].type == CPARSER_WHITESPACE) i--
  return i
}

function cParseDecl (o) {
  return o.returnType + o.name + " (" + o.params.map(it => it.type.trimEnd()).join(', ') + ");"
}

function cParse (s) {
  let decl = extractDecl(s)
  let tokens = tokenize(decl)
  let result = {
    name: '',
    returnType: '',
    params: []
  }
  let i = 0

  while (i < tokens.length) {
    let saveIdx = i

    if (saveIdx !== (i = tkLookahead(tokens, i, CPARSER_OP_LPAR))) {
      result.name = tokens[tkBehind(tokens, i)].val
      break
    }

    result.returnType += tokens[i].val
    i++
  }

  if (result.name.length === 0) {
    throw new Error("position of name not found in `" + s + "`")
  }

  let blocks = 0
  let paramType = ''
  i += 2

  if (tokens[i].type != CPARSER_OP_RPAR) {
    while (i < tokens.length) {
      if (tokens[i].type == CPARSER_OP_RPAR && blocks == 0) {
        if (tokens[i - 1].type == CPARSER_ID && tokens[i - 1].val != paramType) {
          result.params.push({name: tokens[i - 1].val, type: paramType.substr(0, paramType.length - tokens[i - 1].val.length)})
        } else {
          result.params.push({name: "", type: paramType})
        }

        break
      } else if (tokens[i].type == CPARSER_OP_COMMA && blocks == 0) {
        if (tokens[i - 1].type == CPARSER_ID && tokens[i - 1].val != paramType) {
          result.params.push({name: tokens[i - 1].val, type: paramType.substr(0, paramType.length - tokens[i - 1].val.length)})
        } else {
          result.params.push({name: "", type: paramType})
        }

        paramType = ""
        i += 1
        while (i + 1 < tokens.length && tokens[i].type == CPARSER_WHITESPACE) i++
        continue
      } else if (tokens[i].type == CPARSER_OP_LPAR) {
        blocks++
      } else if (tokens[i].type == CPARSER_OP_RPAR) {
        blocks--
      }

      paramType += tokens[i].val
      i++
    }
  }

  return result
}

function apiItemEval (item, code) {
  let name = ''
  let isName = false
  let result = ''

  for (let i = 0; i < code.length; i++) {
    let ch = code[i]

    if (ch == '_' && i + 1 < code.length && code[i + 1] == '{') {
      isName = true
      i += 1
    } else if (isName && ch == '}') {
      if (name !== item.name && !item.fileDependencies.includes(name)) {
        item.fileDependencies.push(name)
      }

      result += name
      isName = false
      name = ''
    } else if (isName) {
      name += ch
    } else {
      result += ch
    }
  }

  return result
}

function parseMetadata () {
  const content = fs.readFileSync(path.join(srcDir, 'codegen-metadata.hpp'), 'utf8')
  const data = content.slice(content.indexOf(' = {' + os.EOL) + 4, content.indexOf(os.EOL + '};' + os.EOL)).trim()
  const lines = data.split(os.EOL).map(it => it.trim().slice(45)).filter(it => it !== '')
  const result = {}

  for (const line of lines) {
    const [_, name, libs] = /\{"([^"]+)", \{([^}]+)\}\},?/g.exec(line)
    result[name] = libs.split(',').map(it => it.trim().slice(1, -1))
  }

  return result
}

main()
