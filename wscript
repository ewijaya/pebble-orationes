import os.path
import subprocess
import sys

top = '.'
out = 'build'


def options(ctx):
    ctx.load('pebble_sdk')


def configure(ctx):
    ctx.load('pebble_sdk')


def build(ctx):
    subprocess.run([sys.executable, 'scripts/generate_text_resource.py', '--check'], check=True)
    ctx.load('pebble_sdk')

    binaries = []
    cached_env = ctx.env

    for platform in ctx.env.TARGET_PLATFORMS:
        ctx.env = ctx.all_envs[platform]
        # Cross-module size optimization keeps the loaded image below Pebble's
        # 16-bit load_size limit while preserving the bundled prayer library.
        ctx.env.append_value('CFLAGS', ['-flto'])
        # The loader requires the generated process header at the start of the image.
        ctx.env.append_value('LINKFLAGS', ['-flto', '-Wl,--undefined=__pbl_app_info'])
        ctx.set_group(ctx.env.PLATFORM_NAME)
        app_elf = '{}/pebble-app.elf'.format(ctx.env.BUILD_DIR)
        ctx.pbl_build(
            source=ctx.path.ant_glob('src/c/**/*.c'),
            target=app_elf,
            bin_type='app'
        )
        binaries.append({'platform': platform, 'app_elf': app_elf})

    ctx.env = cached_env
    ctx.set_group('bundle')
    ctx.pbl_bundle(
        binaries=binaries,
        js=ctx.path.ant_glob(['src/pkjs/**/*.js',
                              'src/pkjs/**/*.json',
                              'src/common/**/*.js']),
        js_entry_file='src/pkjs/index.js'
    )
