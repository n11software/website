import '../styles/globals.css'
import type { AppProps } from 'next/app'
import Head from 'next/head'
import { getCookie, getCookies, setCookies } from 'cookies-next'
import { useEffect } from 'react'

let Page = ({ Component, pageProps }: AppProps) => {
  useEffect(()=>{
    document.querySelector("#bypass").addEventListener("click", function(e) {
      e.preventDefault();
      setCookies("bypass", "true")
      window.location.reload()
    }, false);
    if (getCookie("bypass")) document.querySelector("#warning").classList.add("hidden")
  },[])
  return <>
    <Head>
      <link rel="apple-touch-icon" sizes="180x180" href="/apple-touch-icon.png"/>
      <link rel="icon" type="image/png" sizes="32x32" href="/favicon-32x32.png"/>
      <link rel="icon" type="image/png" sizes="16x16" href="/favicon-16x16.png"/>
      <link rel="manifest" href="/site.webmanifest"/>
      <link rel="mask-icon" href="/safari-pinned-tab.svg" color="#000000"/>
      <meta name="msapplication-TileColor" content="#000000"/>
      <meta name="theme-color" content="#ffffff"/>
      <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no"/>
    </Head>
    <div className="flex flex-col z-50 w-screen h-screen items-center justify-center text-center fixed top-0 left-0 right-0 bottom-0 bg-white text-red-500" id="warning">
      <span className="text-5xl font-black mb-2">Warning</span>
      <span className="text-2xl w-3/4 text-stone-600">This website is still under construction!</span>
      <span className="text-2xl w-3/4 text-stone-600 mb-6">Do you still want to continue?</span>
      <a className="text-xl px-6 py-1 bg-red-500 rounded text-white cursor-pointer" id="bypass">Continue</a>
    </div>
    <Component {...pageProps} />
  </>
}

export default Page