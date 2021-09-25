#!/bin/sh

#
# Copyright (c) Aaron Delasy
#
# Unauthorized copying of this file, via any medium is strictly prohibited
# Proprietary and confidential
#

cat > .git/hooks/pre-commit << EOF
#!/bin/sh
make
EOF

chmod +x .git/hooks/pre-commit
