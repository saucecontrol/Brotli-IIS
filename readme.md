Brotli IIS Compression Scheme Plugin
====================================

Brotli is a new-ish open-sourced compression algorithm specifically designed for HTTP content encoding.  The algorithm and reference encoder/decoder libraries were [created by Google](https://github.com/google/brotli) and offered for free.

Brotli offers significantly [better compression than gzip](https://samsaffron.com/archive/2016/06/15/the-current-state-of-brotli-compression) with very little additional compression cost and almost no additional decompression cost.

This IIS Compression Scheme plugin is free and open source, in keeping with the philosophy of Brotli itself.

This plugin is a very thin wrapper around Google's Brotli encoding library.  There is no license management code, no automagic configuration, no unnecessary procecessing.  This plugin contains only what is absolutely necessary to cleanly integrate Google's Brotli encoder with IIS's built-in Static and Dynamic Compression Modules.

Of course, that means you have to configure it yourself.  But a proper HTTP compression design requires that you know what you're doing anyway, so this should not be a problem.  If you're new to this, you may find the following links useful for learning about IIS compression and the configuration thereof.

* [Configuring HTTP Compression in IIS 7](https://docs.microsoft.com/en-us/previous-versions/windows/it-pro/windows-server-2008-R2-and-2008/cc771003(v%3dws.10))
* [IIS 7 Compression. Good? Bad? How much?](https://weblogs.asp.net/owscott/iis-7-compression-good-bad-how-much)
* [Changes to compression in IIS7](http://www.ksingla.net/2006/06/changes_to_compression_in_iis7/)
* [Built-in GZip/Deflate Compression on IIS 7.x](https://weblog.west-wind.com/posts/2011/May/05/Builtin-GZipDeflate-Compression-on-IIS-7x)

Very little has changed since IIS 7 was released, but here's one more article highlighting some improvements to dynamic compression in IIS 10
* [IIS Dynamic Compression and new Dynamic Compression features in IIS 10](https://blogs.msdn.microsoft.com/friis/2017/09/05/iis-dynamic-compression-and-new-dynamic-compression-features-in-iis-10/)

Features
--------

* Works with the built-in IIS Static and Dynamic Compression Modules.
* Uses the latest version of Google's Brotli encoder (v1.02).

Requirements
------------

IIS 7.5 or later (Windows 7/Windows Server 2008 R2), x64 only. You must have admin permissions to modify the root applicationHost.config file.

Installation
------------

The Brotli IIS Compression Scheme Plugin is packaged as a single DLL file with no external dependencies.  The simplest way to install it is to copy it to your `inetsrv` folder, alongside the built-in `gzip.dll`.

Binaries for x64 Windows are available on the [releases page](https://github.com/saucecontrol/BrotliIIS/releases).

The Compression Scheme must be registered in the `applicationHost.config` file.  You can do this manually or with AppCmd.exe or IIS Manager.  Final configuration will look something like this:

```
<httpCompression directory="%SystemDrive%\inetpub\temp\IIS Temporary Compressed Files">
    <scheme name="br" dll="%Windir%\system32\inetsrv\brotli.dll" dynamicCompressionLevel="5" staticCompressionLevel="11" />
    <scheme name="gzip" dll="%Windir%\system32\inetsrv\gzip.dll" dynamicCompressionLevel="4" staticCompressionLevel="9" />
    <staticTypes>
        <add mimeType="text/*" enabled="true" />
         ...
    </staticTypes>
    <dynamicTypes>
        <add mimeType="text/*" enabled="true" />
        ...
    </dynamicTypes>
</httpCompression>
```

Note: The name `br` shown above is important.  This name must match the `Accept-Encoding` header value sent by the client and will be returned to the client in the `Content-Encoding` header.  `br` is the official designator for Brotli.

Browser Support
---------------

As of the end of 2017, Brotli is supported in [all modern browsers](https://caniuse.com/#feat=brotli).

There are however, some gotchas related to the way the browser support is implemented.

### HTTPS is Required

Current browsers will only request and accept Brotli encoding over HTTPS.  Due to some poorly-behaved intermediate software/devices (proxies, caches, etc) in the wild, the [Chrome team decided](https://bugs.chromium.org/p/chromium/issues/detail?id=452335#c87) to only advertise Brotli support over HTTPS to make sure these poorly-behaved intermediate software and devices couldn't interfere with Brotli-encoded responses.  Other vendors followed suit.

If you aren't using HTTPS, you can't use Brotli.  Thankfully, with [Let's Encrypt](https://github.com/Lone-Coder/letsencrypt-win-simple), HTTPS is now free and easy to set up.  Just do it.

### Brotli is Low-Priority

Current browsers advertise Brotli support with a lower priority than gzip or deflate.  Typical `Accept-Encoding` headers will be `Accept-Encoding: gzip, deflate, br`.  This is probably also for reasons related to existing poorly-behaved Internet software.

However, because the IIS Compression Modules are well-behaved and respect the client's requested encoding priority, they will not choose Brotli if `gzip` or `deflate` compression is also configured on your server.  It is desirable to keep `gzip` enabled to support non-Brotli clients, so if you want to force Brotli, you will need to modify the requests as they enter your IIS pipeline.  The [IIS URL Rewrite Module](https://www.iis.net/downloads/microsoft/url-rewrite) makes this easy.

You'll need to enable rewrite of the `Accept-Encoding` header and then configure a rule to prioritize `br` for clients that support it.  Here is a sample configuration:

```
<rewrite>
    <allowedServerVariables>
        <add name="HTTP_ACCEPT_ENCODING" />
    </allowedServerVariables>
    <rule name="Prioritize Brotli">
        <match url=".*" />
        <conditions>
            <add input="{HTTP_ACCEPT_ENCODING}" pattern="\bbr\b" />
        </conditions>
        <serverVariables>
            <set name="HTTP_ACCEPT_ENCODING" value="br" />
        </serverVariables>
    </rule>
</rewrite>
```

This rule simply looks for the string `br` (surrounded by word boundaries) in the `Accept-Encoding` header and re-writes it to be just `br`, removing the choice of others.  With only one choice of valid `Response-Encoding`, the IIS Compression Modules will be forced to select Brotli.  This allows you to leave `gzip` and/or `deflate` enabled for clients that do no advertise `br` support.
