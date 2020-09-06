#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# Define colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'

cd $GITHUB_WORKSPACE

# Install doxygen
echo -e "\n########################################################################";
echo -e "${YELLOW}INSTALL DOXYGEN"
echo "########################################################################";
wget -q https://downloads.sourceforge.net/project/doxygen/rel-1.8.18/doxygen-1.8.18.linux.bin.tar.gz
tar -xf doxygen-1.8.18.linux.bin.tar.gz
mv doxygen-1.8.18/bin/doxygen .
chmod +x doxygen

# Create a clean working directory for this script
mkdir doxygen_workdir
cd doxygen_workdir

# Get the current gh-pages branch
echo -e "\n########################################################################";
echo -e "${YELLOW}CLONE REPO"
echo "########################################################################";
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
echo -e "\n########################################################################";
echo -e "${YELLOW}DELETE CURRENT DOCS"
echo "########################################################################";
rm -rf api || true

# Overwrite output directory
echo -e "\n########################################################################";
echo -e "${YELLOW}GENERATE DOCS"
echo "########################################################################";
cp Doxyfile ${GITHUB_WORKSPACE}/Doxyfile
sed -i "s;^HTML_OUTPUT .*;HTML_OUTPUT = doxygen_workdir/${REPO_NAME}/api;" ${GITHUB_WORKSPACE}/Doxyfile
cd $GITHUB_WORKSPACE

# Create documentation
$GITHUB_WORKSPACE/doxygen Doxyfile

#Check if master branch
if [ "${GITHUB_REF}" != "refs/heads/master" ]; then
    echo -e "${GREEN}Not on master branch, not going to commit to gh-pages!"
    exit 0
fi

# Commit documentation
cd doxygen_workdir/${REPO_NAME}

if [ -d "api" ] && [ -f "api/index.html" ]; then
    git add --all

    if [ -n "$(git status --porcelain)" ]; then
        echo -e "\n########################################################################";
        echo -e "${YELLOW}COMMIT CHANGES"
        echo "########################################################################";
    else
        echo -e "${GREEN}No changes to commit"
        exit 0
    fi

    git commit -m "Deploy code docs to github pages ${GITHUB_RUN_NUMBER}" -m "Commit: ${GITHUB_SHA}"

    git push --force "https://${GITHUB_ACTOR}:${GH_REPO_TOKEN}@github.com/${GITHUB_REPOSITORY}.git" > /dev/null 2>&1
else
    echo -e "${RED}Error: No documentation files have been found!" >&2
    exit 1
fi