import Navbar from '../mods/Navbar/Navbar'
import Head from 'next/head'
import ApplyButton from '../mods/Navbar/Apply/Button'

let Index = () => {
  return <>
    <Head>
      <title>N11</title>
    </Head>
    <Navbar/>
    <div className="w-screen h-screen flex">
      <div className="flex flex-col text-stone-800 w-full pt-32 pl-24 pr-24">
        <span className="sm:text-5xl text-3xl font-black mb-2">Optimized, secure, and modern products.</span>
        <span className="sm:text-2xl text-xl w-3/4 text-stone-600 mb-12">N11 is a business focused on creating things from nothing making them secure, fast, and modernized.</span>
        <ApplyButton/>
      </div>
      <div className="flex-col text-white w-full items-end justify-center pr-24 pt-32 pb-12 sm:flex hidden">
        <img src="/HeroNuclear.png" className="h-full w-full object-scale-down"/>
      </div>
    </div>
    <img src="Waves/White-Black.svg" className="w-full sm:h-48 h-16"/>
    <div className="w-screen flex flex-col bg-black h-screen">
      <span className="text-5xl font-black text-white pl-32 pt-20"></span>
    </div>
    <img src="Waves/Black-White.svg" className="w-full sm:h-48 h-16"/>
  </>
}

export default Index