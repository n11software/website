import Link from 'next/link'

let DonateButton = props => {
  return <Link href="/donate">
    <a className="flex cursor-pointer space-x-2 items-center py-2 px-2 rounded-lg transition-colors active:bg-black active:text-gray-200 text-gray-800 hover:bg-stone-300">
      Donate
    </a>
  </Link>
}
export default DonateButton