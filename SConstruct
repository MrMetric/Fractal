#!python3
import os

target = ARGUMENTS.get('target', 'debug')

TARGET_FLAGS = {
	'debug': [
		'-g3',
		'-Og',
	],
	'reldbg': [
		'-g3',
		'-O2',
	],
	'release': [
		'-O2',
	],
}

if target == 'release':
	FSANITIZE = []
else:
	FSANITIZE = [
		'-fsanitize=address,undefined,unsigned-integer-overflow,nullability',
		'-fno-sanitize-recover=null',
	]

WARNING_FLAGS = [
	'-Weverything',
	'-Werror=cast-qual',
	#'-Werror=conversion',
	'-Werror=delete-incomplete',
	'-Werror=delete-non-virtual-dtor',
	'-Werror=deprecated',
	'-Werror=extra-semi',
	'-Werror=extra-tokens',
	'-Werror=float-conversion',
	'-Werror=format',
	'-Werror=implicit-fallthrough',
	'-Werror=inconsistent-missing-destructor-override',
	'-Werror=inconsistent-missing-override',
	'-Werror=invalid-pp-token',
	'-Werror=mismatched-new-delete',
	'-Werror=mismatched-tags',
	'-Werror=missing-declarations',
	'-Werror=missing-field-initializers',
	#'-Werror=missing-prototypes',
	'-Werror=multichar',
	'-Werror=newline-eof',
	'-Werror=non-virtual-dtor',
	#'-Werror=old-style-cast',
	'-Werror=return-stack-address',
	'-Werror=return-type',
	'-Werror=self-move',
	#'-Werror=shorten-64-to-32',
	'-Werror=sign-compare',
	'-Werror=sign-conversion',
	'-Werror=static-inline-explicit-instantiation', # note: GCC says that `inline template<>` is invalid syntax
	'-Werror=string-compare',
	'-Werror=strict-prototypes',
	'-Werror=string-compare',
	'-Werror=string-plus-int',
	'-Werror=undefined-reinterpret-cast',
	'-Werror=uninitialized',
	'-Werror=unknown-pragmas',
	'-Werror=unknown-warning-option',
	'-Werror=unused-result',
	'-Werror=weak-vtables',
	'-Werror=zero-as-null-pointer-constant',
	'-Wno-c++98-compat',
	'-Wno-c++98-compat-pedantic',
	'-Wno-comment',
	'-Wno-covered-switch-default',
	'-Wno-deprecated-dynamic-exception-spec',
	'-Wno-documentation-unknown-command',
	'-Wno-double-promotion',
	'-Wno-exit-time-destructors',
	'-Wno-float-equal',
	'-Wno-global-constructors',
	'-Wno-logical-op-parentheses',
	'-Wno-missing-braces',
	'-Wno-padded',
	'-Wno-shadow',
	'-Wno-undefined-func-template',
]

CXXFLAGS = [
	'-std=c++17',
	'-iquote', 'src',
	'-isystem', 'lib',

	'-fno-omit-frame-pointer',
	'-fno-strict-aliasing',
	'-fstack-protector-strong',
	'-fvisibility=hidden',
	'-DVISIBLE=\'__attribute__((visibility("default")))\'',
]
CXXFLAGS += TARGET_FLAGS[target]
CXXFLAGS += FSANITIZE
CXXFLAGS += WARNING_FLAGS

LINKFLAGS = [
	'-flto',
]
LINKFLAGS += FSANITIZE

LIBS = [
	'libpng',
	'stdc++fs',
]

PKG_CONFIG_LIBS = [
]

env = Environment()
env['ENV']['TERM'] = os.environ['TERM']
env['CC'] = 'clang'
env['CXX'] = 'clang++'
env.Append(CXXFLAGS = CXXFLAGS)
env.Append(LINKFLAGS = LINKFLAGS)
env.Append(LIBS = LIBS)
if len(PKG_CONFIG_LIBS) != 0:
	env.ParseConfig('pkg-config --cflags --libs ' + ' '.join(PKG_CONFIG_LIBS))

def get_src(*dirs):
	src = []
	for d in dirs:
		for root, subdirs, files in os.walk(d):
			for file in files:
				if file.endswith('.cpp'):
					src.append(os.path.join(root, file))
	return src
sources = get_src('lib', 'src')
target = env.Program(target='fractal', source=sources)
Default(target)
