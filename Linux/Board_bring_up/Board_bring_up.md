# Board Bring-Up

## Step 1: Login to Remote PC

```bash
ssh test@10.3.21.172
# Type 'yes' to accept host key
# Password: ifx
```

## Step 2: Navigate to Yocto workspace

```bash
cd imx
cd yocto
mkdir test
ls
```

## Step 3: Build ARM toolchain using Yocto (summary)

```bash
# Go to Yocto workspace
cd ~/imx/yocto

# Initialize build environment (use the board setup script used in your Yocto tree)
source setup-environment build-imx8mq

# Set target machine if not already set by setup script
export MACHINE=imx8mqea-com

# Optional: verify layer configuration
bitbake-layers show-layers

# Build image first (common prerequisite)
bitbake core-image-minimal

# Build SDK/toolchain from image
bitbake core-image-minimal -c populate_sdk

# SDK installer output path
ls tmp/deploy/sdk/

# Install generated SDK on host
chmod +x tmp/deploy/sdk/*.sh
./tmp/deploy/sdk/*.sh
```

## Step 4: Validate installed toolchain

```bash
# Load SDK environment (path depends on installer output)
source /opt/*environment-setup*

# Check cross compiler
which aarch64-poky-linux-gcc
aarch64-poky-linux-gcc --version
```

## Notes

- If your Yocto project uses a different image name, replace `core-image-minimal` with that image.
- If your board machine name differs, replace `imx8mqea-com` with the machine used in your build.
