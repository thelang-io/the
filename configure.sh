#!/bin/sh -e

#
# Copyright (c) Aaron Delasy
#
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#

if [ "$1" = "build" ]; then
  make
elif [ "$1" = "mount-git-hooks" ]; then
  cat > .git/hooks/pre-commit << EOF
#!/bin/sh
make
EOF

  chmod +x .git/hooks/pre-commit
elif [ "$1" = "--help" ] || [ "$1" = "--h" ]; then
  echo
  echo "  Usage:"
  echo
  echo "     ./configure.sh [option]"
  echo "     ./configure.sh [action]"
  echo
  echo "  Options:"
  echo
  echo "    -h, --help        Print help information"
  echo
  echo "  Actions:"
  echo
  echo "    build             Build the core"
  echo "    mount-git-hooks   Mount Git hooks"
  echo

elif [ "$1" = "" ]; then
  echo "Operation is not specified" 1>&2
  exit 1
else
  echo "Unknown operation \"$1\"" 1>&2
  exit 1
fi
