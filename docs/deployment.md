# Landing Deployment

The landing page is a static site under `landing/`. Do not expose the repository root.

## Cloudflare Pages

Production URL:

```text
https://buffleaudio-align.pages.dev/
```

Project name:

```text
buffleaudio-align
```

Deploy:

```bash
npx wrangler@latest pages deploy landing --project-name=buffleaudio-align --branch=main
```

`wrangler.toml` points Pages at `landing/`:

```toml
name = "buffleaudio-align"
pages_build_output_dir = "landing"
```

Cloudflare Pages project names must use lowercase characters and dashes. A hostname like `buffleaudio.align.pages.dev` is not valid as a Pages project subdomain because the project segment contains a dot.

## Cloudflare Tunnel

Serve only the landing folder locally:

```bash
scripts/serve_landing.sh
```

Expose it with a quick tunnel:

```bash
scripts/expose_landing_cloudflared.sh
```

The tunnel is useful for live preview, but Cloudflare Pages is the stable public deployment.

## Support Link

The landing page includes support CTAs to:

```text
https://buymeacoffee.com/hostin.tech
```
