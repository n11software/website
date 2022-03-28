import '../styles/globals.css'
import type { AppProps } from 'next/app'
let Page = ({ Component, pageProps }: AppProps) => { return <Component {...pageProps} /> }
export default Page