{
    "targets": [
        {
            "target_name": "wrapper",
            "sources": [ 
                # "src/index.cc",
                "src/wrapper.cc" ,
                # "src/unplug.cc" ,
                "src/debugger.cc" 
            ],
            'include_dirs': [
                "<!(node -e \"require('nan')\")"
            ],
            "ldflags": [
                "-Wl,-z,defs"
            ]
            # 'dependencies': [],
            # "cflags!": [ "-fno-exceptions" ],
            # "cflags_cc!": [ "-fno-exceptions" ],
            # 'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
        }
    ]
}