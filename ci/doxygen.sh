#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

cd $GITHUB_WORKSPACE

# Install doxygen
wget -q http://doxygen.nl/files/doxygen-1.8.18.linux.bin.tar.gz
tar -xf doxygen-1.8.18.linux.bin.tar.gz
mv doxygen-1.8.18/bin/doxygen .
chmod +x doxygen

# Create a clean working directory for this script
mkdir doxygen_workdir
cd doxygen_workdir

# Get the current gh-pages branch
git clone -b gh-pages https://github.com/${GITHUB_REPOSITORY}.git
export REPO_NAME=${GITHUB_REPOSITORY#*/}
cd ${REPO_NAME}

# Configure git.
# Set the push default to simple i.e. push only the current branch.
git config --global push.default simple
# Pretend to be an user called Doxygen CI.
git config user.name "Doxygen CI"
git config user.email "doxygen@github.com"

# Delete current docs
rm -rf html || true

# Overwrite output directory
cp Doxyfile ${GITHUB_WORKSPACE}/Doxyfile
sed -i "s;^HTML_OUTPUT .*;HTML_OUTPUT = doxygen_workdir/${REPO_NAME}/html;" ${GITHUB_WORKSPACE}/Doxyfile
cd $GITHUB_WORKSPACE

# Create documentation
$GITHUB_WORKSPACE/doxygen Doxyfile

cd doxygen_workdir/${REPO_NAME}

if [ -d "html" ] && [ -f "html/index.html" ]; then
    git add --all

    if [ -n "$(git status --porcelain)" ]; then
        echo "Changes to commit"
    else
        echo "No changes to commit"
        exit 0
    fi

    git commit -m "Deploy code docs to github pages ${GITHUB_RUN_NUMBER}" -m "Commit: ${GITHUB_SHA}"

    git push --force "https://${GITHUB_ACTOR}:${GH_REPO_TOKEN}@github.com/${GITHUB_REPOSITORY}.git" > /dev/null 2>&1
else
    exit 1
fi