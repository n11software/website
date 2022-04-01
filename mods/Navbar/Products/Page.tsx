import Link from 'next/link'

let ProductsPage = props => {
  return props.bool ? <div className="fixed z-0 top-14 left-0 right-0 w-full flex h-full text-gray-800">
    <div className="flex flex-col w-1/4 bg-stone-100 p-4">
      <span className="text-md uppercase font-bold mb-8">Featured</span>
      <div className="flex flex-col space-y-2 overflow-y-auto mb-16">
        <Link href="/products/lambda">
          <a className="h-18">
            <div className="flex flex-col px-4 py-2 transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Lambda</span>
              <span className="text-lg text-gray-600 font-light">A game engine.</span>
            </div>
          </a>
        </Link>
        <Link href="/products/kaos">
          <a className="h-18">
            <div className="flex flex-col px-4 py-2 transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Kaos</span>
              <span className="text-lg text-gray-600 font-light">A platform for gamers.</span>
            </div>
          </a>
        </Link>
        <Link href="/products/hydra">
          <a className="h-18">
            <div className="flex flex-col px-4 py-2 transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Hydra</span>
              <span className="text-lg text-gray-600 font-light">A modern Operating System.</span>
            </div>
          </a>
        </Link>
        <Link href="/products/nuclear">
          <a className="h-18">
            <div className="flex flex-col px-4 py-2 transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Nuclear</span>
              <span className="text-lg text-gray-600 font-light">A low level language.</span>
            </div>
          </a>
        </Link>
        <Link href="/products/neon">
          <a className="h-18">
            <div className="flex flex-col px-4 py-2 transition-colors hover:bg-stone-300 active:bg-black active:text-gray-200 rounded-xl">
              <span className="text-xl font-bold">Neon</span>
              <span className="text-lg text-gray-600 font-light">A version control system.</span>
            </div>
          </a>
        </Link>
      </div>
    </div>
    <div className="flex h-full w-full">
      <div className="flex flex-col h-full w-full">
        <div className="flex flex-col w-full px-8 py-4">
          <span className="text-md uppercase font-bold mb-4">Gaming</span>
          <div className="flex flex-col w-full h-full space-y-2">
            <Link href="/products/kaos">
              <a className="text-lg text-gray-600 font-light hover:text-black">Kaos</a>
            </Link>
            <Link href="/products/lambda">
              <a className="text-lg text-gray-600 font-light hover:text-black">Lambda</a>
            </Link>
            <Link href="/products/n11client">
              <a className="text-lg text-gray-600 font-light hover:text-black">N11 Client</a>
            </Link>
            <Link href="/products/nmc">
              <a className="text-lg text-gray-600 font-light hover:text-black">NotMinecraft</a>
            </Link>
          </div>
        </div>
        <div className="flex flex-col w-full h-full px-8 py-4">
          <span className="text-md uppercase font-bold mb-4">Coding</span>
          <div className="flex flex-col w-full h-full space-y-2">
            <Link href="/products/lambda">
              <a className="text-lg text-gray-600 font-light hover:text-black">Lambda</a>
            </Link>
            <Link href="/products/hydra">
              <a className="text-lg text-gray-600 font-light hover:text-black">Hydra</a>
            </Link>
            <Link href="/products/nuclear">
              <a className="text-lg text-gray-600 font-light hover:text-black">Nuclear</a>
            </Link>
            <Link href="/products/neon">
              <a className="text-lg text-gray-600 font-light hover:text-black">Neon</a>
            </Link>
            <Link href="/products/link">
              <a className="text-lg text-gray-600 font-light hover:text-black">Link</a>
            </Link>
          </div>
        </div>
      </div>
      <div className="flex flex-col h-full w-full">
        <div className="flex flex-col w-full h-full px-8 py-4">
          <span className="text-md uppercase font-bold mb-4">Servers</span>
          <div className="flex flex-col w-full h-full space-y-2">
            <Link href="/products/xeon">
              <a className="text-lg text-gray-600 font-light hover:text-black">Xeon</a>
            </Link>
            <Link href="/products/link">
              <a className="text-lg text-gray-600 font-light hover:text-black">Link</a>
            </Link>
          </div>
        </div>
      </div>
    </div>
  </div> : <></>
}

export default ProductsPage